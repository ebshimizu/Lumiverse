var express = require('express');
var app = express();

// swig connection
var swig = require('./build/Release/dumiverse_swig');

// Get command line arguments
if (process.argv.length < 4) {
    console.log("usage: node dumiverse <port> <path to plugins dir>");
    process.exit(1);
}

// Port number service is listening on
var port = process.argv[2];

// Arnold values passed in POST request
var path = require('path');
var m_buffer_output = path.join(__dirname, "libbuf_out.buf");
var render_done = path.join(__dirname, "render_done.out");
var arnoldOutput = null;
var m_gamma = null;
var m_predictive = null;

// Path to temporary ass file
var assFile = './tmp_ass/tmp.ass';

// Unique connection id enumeration
var connectionId = 0;

// Stream buffers
var fs = require('fs');
var StreamBuffers = require('stream-buffers');

var connectionOpen = false;

// Object for reading parameters
var multer = require('multer');
var storage = multer.memoryStorage(); // Store ass files in memory
var upload = multer({ storage: storage });

var Busboy = require('busboy');

// Handle for zipping and unzipping
const zlib = require('zlib');

/**
 * Tell dumiverse to listen on the port specified on the command line
 */
app.listen(port, function () {
    console.log('Dumiverse listening for new connections on port: ' + port);
});

/**
 * Function for seeing what endpoints are available
 */
app.get('/', function( req, res ) {
    res.send(
        'Endpoint options for Dumiverse (distributed Lumiverse renderer):\n' +
        '/open --- Open a connection to this renderer (only one connection allowed at a time)\n' +
        '/init -F "ass_file=@ass_file.gz" -F "m_patch={json_patch}" -- Initialize an open connection\n' +
        '/render --- -F "m_parameters={devices_json}" -F "m_settings={settings_json} Render a scene and return buffer\n' +
        '/interrupt --  Interrupt a currently executing render\n' +
        '/percent -- Get the current percentage of rendering that has completed' +
        '/close --- Close / free an open connection\n\n' +
        'NOTE: The remote renderer must be told what plugins it needs, and all references your .ass file must point to files on the remote server'
    );
});

/**
 * Open a lumiverse connection to dumiverse
 */
app.get('/open', function (req, res) {
    if( connectionOpen ) {
        res.json({ 
                success: false,
                msg: "Connection already open. Please close before attempting to open a new connection"
        });

        return
    }


    connectionOpen = true;
    res.json({
            success: true,
            msg: "Connection successfully opened. Waiting for ass file and parameters."
    });
    console.log("Opened new connection with endpoint ");
});

/* 
 * Initialize a previously opened connection. This includes
 * receiving an Arnold ass file, loading it into memory, and
 * initializing any data structures
 */
app.post('/init', upload.single('ass_file'), function (req, res) {
    if( !connectionOpen ) {
        res.json({ 
                success: false,
                msg: "Connection has not been opened. Please open a connection before sending an ass file and parameters"
        });

        return
    }

    // Get arnold parameters out of the request
    var fileBuffer = req.file.buffer;
    var m_patch = req.body.m_patch;

    if( m_patch == null || fileBuffer == null ) {
        res.status(500).json({
                success: false,
                msg: 'Missing request parameters. Need m_patch and a gzipped ass file',
                m_patch: m_patch == null,
                file_buffer: fileBuffer == null
        });

        return;
    }

    var unzippedBuffer = fileBuffer;
    try {
        unzippedBuffer = zlib.gunzipSync(fileBuffer);
    } catch( e ) {
        console.log('Unable to gunzip file -- assuming it is not gzipped');
        unzippedBuffer = fileBuffer;
    }

    fs.writeFileSync(assFile, unzippedBuffer);

    swig.init(m_patch, assFile);

    var width = swig.getWidth();
    var height = swig.getHeight();

    res.json({
        success: true,
        m_width: width,
        m_height: height,
        msg: 'Successfully receieved file and initialized Arnold. Waiting for requests to /render endpoint'
    });
    return;
});


/**
 * Helper function for checking what ass file is currently
 * loaded
 */
app.get('/check_buffer', function( req, res ) { 
    if( !connectionOpen ) {
        res.status(403).json({ 
                success: false,
                msg: "Unable to check buffer. Either connection is not opened or no assFile has been received",
        });

        return;
    }

    var frameBuffer = swig.getFrameBuffer();
    console.log(frameBuffer);

    res.json({
        success: true,
        msg: 'Buffer checked'
    });
});

/**
 * Helper function for interrupting a currently executing
 * render call
 */
app.get('/interrupt', function (req, res) {
    if (!connectionOpen) {
        res.status(403).json({
            success: false,
            msg: "Unable to interrupt rendering -- no connection open",
        });

        return;
    }

    swig.interrupt();

    res.json({
        success: true,
        msg: 'Interrupted rendering'
    });
});

/**
 * Helper function for interrupting a currently executing
 * render call
 *
 * @TODO: Actually get this working. Need to figure out what data
 * structures to maintain on the dumiverse side
 */
app.get('/percent', function (req, res) {
    if (!connectionOpen) {
        res.status(403).json({
            success: false,
            msg: "Unable to get rendering percentage -- no connection open",
        });

        return;
    }

    var percent = swig.getPercentage();

    res.json({
        success: true,
        msg: 'Got rendering progress status.',
        percent: percent
    });
});

/**
 * Receive request to render a file.
 * Render the file, fill the frame buffer, and then zip and return it.
 */
app.post('/render', upload.single('ass_file'), function (req, res) {
    // Verify connection open
    if( !connectionOpen ) {
        console.log('Received a request to render when a connection had not been opened');
        res.status(403).json({
            success: false,
            msg: 'Error. You must open and initialize a connection before you try to render an image'
        });
        
        return;
    }


    // If there are updated devices then handle it here
    var m_parameters = req.body.m_parameters;
    var m_settings = req.body.m_settings;

    // Render the file and get the response
    var renderResponse = swig.renderWrapper(m_parameters, m_settings);
    if (renderResponse == 0) {
        console.log('Waiting for rendering to complete. Spinning and checking file');

        var doneRendering = false;
        var checkRenderInterval = setInterval(function() {
            try {
                fs.accessSync(render_done, fs.F_OK);
                clearInterval(checkRenderInterval);

                var renderStream = fs.createReadStream(render_done);
                renderStream.on("data", function(chunk) {
                    var renderValue = chunk;
                    console.log('Rendering complete -- render return status: ' + renderValue);
                    if( chunk == 0 ) {
                        /*
                        var frameBuffer = fs.readFileSync(m_buffer_output);
                        var zippedFrameBuffer = zlib.gzipSync(frameBuffer);
                        res.send(zippedFrameBuffer, 'binary');
                        */
                        res.sendFile(m_buffer_output);
                    } else {
                        res.status(500).json({
                            success: false,
                            msg: 'Render failed. Error message: ' + renderValue 
                        });
                    }

                });

                renderStream.on("end", () => {
                    fs.unlinkSync(render_done);

                    return;
                });

            } catch( e ) {}
        }, 100);

        return;
    } else {

        res.status(500).json({
            success: false,
            msg: 'Render failed. Error message: ' + renderResponse
        });
    }
});

/**
 * Close an open lumiverse connection
 */
app.get('/close', function (req, res) {
    if( !connectionOpen ) {
        res.json({ 
            success: false,
            msg: "Connection not open so it can't be closed."
        });

        return
    }

    console.log("Closing connection\n");

    connectionOpen = false;

    /* 
     * Clean up temporary files. Check if they exist before deleting
     * them so we don't crash node. Note that node doesn't have a good
     * way to check if a file exists so we have to wrap every check
     * in a try-catch block (http://devdocs.io/node/fs#fs_fs_access_path_mode_callback)
     */
    try {
        fs.accessSync(assFile, fs.F_OK);
        fs.unlinkSync(assFile);
    } catch( e ) {}

    try {
        fs.accessSync(m_buffer_output, fs.F_OK);
        fs.unlinkSync(m_buffer_output);
    } catch( e ) {}

    if( arnoldOutput != null ) {
        try {
            fs.accessSync(arnoldOutput, fs.F_OK);
            fs.unlinkSync(arnoldOutput);
        } catch( e ) {}
    }

    swig.close();
    m_predictive = null;
    m_gamma = null;
    arnoldPath = null;

    res.json({
        success: true,
        msg: "Connection successfully closed."
    });
});

