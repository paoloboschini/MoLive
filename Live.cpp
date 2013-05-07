/* Copyright (C) 2013 Paolo Boschini */

#include "Live.h"
#include "ResourceDownloader.h"

/**
 * Constructor.
 */
Live::Live() : DEFAULT_URL("Insert url here") {
    initialize();
    String url = DEFAULT_URL;
    String fileName = mFileUtil->getLocalPath() + "LastUrl.txt";

    // Get the most recently used url.
    bool success = mFileUtil->readTextFromFile(fileName, url);
    mUrlField->setText(url);
    if (success) showPage(url);

    // debug
    lprintfln("@@@ Filename is %s", fileName.c_str());

    // Register function to handle custom messages sent from JavaScript.
    addMessageFun(
            "restoreWebView",
            (FunTable::MessageHandlerFun)&Live::restoreWebView);

    // Register function to handle custom messages sent from JavaScript.
    addMessageFun(
            "downloadResource",
            (FunTable::MessageHandlerFun)&Live::downloadResource);

    addMessageFun(
            "getListResources",
            (FunTable::MessageHandlerFun)&Live::getListResources);

    // Create resource folder if it does not exist
    fileCreate(mFileUtil->getLocalPath() + "resources/");
}

/**
 * Called from JavaScript to restore the main WebView.
 * This method is useful to clean up Native UI elements
 * that reside on top of the main WebView.
 *
 * @param message The stream sent from JavaScript.
 */
void Live::restoreWebView(Wormhole::MessageStream& message) {
    mScreen->show();
}

/**
 * Called from JavaScript to download a resource.
 *
 * @param stream The stream sent from JavaScript.
 */
void Live::downloadResource(Wormhole::MessageStream& stream) {
    // Get download url.
    const char* url = stream.getNext();
    const char* filename = stream.getNext();
    const char* callbackId = stream.getNext();

    // Initiate download. The downloader will delete itself.
    (new ResourceDownloader(filename, callbackId, getWebView()))->get(url);
}

/**
 * Called from JavaScript to get the resources on this
 * device. Resources reside in the resource folder.
 *
 * @param stream The stream sent from JavaScript.
 */
void Live::getListResources(Wormhole::MessageStream& stream) {
    const char *callbackId = stream.getNext();
    List<String> files = getFileList((mFileUtil->getLocalPath() + "resources").c_str());
    String result = listToJavaScriptArray(files);
    printf("result: %s", result.c_str());
    String script = String("mosync.bridge.reply(") + callbackId + "," + result + ");";
    mWebView->callJS(script);
}

/**
 * Gets a file list contained in the given directory.
 *
 * @param path  The path where to look for the file list.
 * @return      The list of files in the given path.
 */
List<String> Live::getFileList(const char* path) {
    // The buffer is static to conserve stack space.
    static char buffer[1024];
    List<String> files;
    MAHandle list = maFileListStart(path, "*", NULL);

    if(list < 0) {
        printf("maFileListStart error %i", list);
        return files;
    }
    int size;
    while((size = maFileListNext(list, buffer, sizeof(buffer))) > 0) {

        if(size > (int)sizeof(buffer)) {
            printf("Buffer too small!\n");
            return files;
        }

        files.addLast(buffer);
    }
    maFileListClose(list);
    return files;
}

/**
 * Creates a file or a directory.
 *
 * @param path  The file or directory to create
 * @return      True on success, false on fail.
 */
bool Live::fileCreate(const String& path) {
    MAHandle file = maFileOpen(path.c_str(), MA_ACCESS_READ_WRITE);
    if (file < 0) return false;
    int result = maFileCreate(file);
    maFileClose(file);
    return 0 == result;
}

/**
 * Check that a file exists.
 *
 * @param path  The file or directory to check.
 * @return      True if path exists, false if it does not exist.
 */
bool Live::fileExists(const String& path) {
    MAHandle file = maFileOpen(path.c_str(), MA_ACCESS_READ_WRITE);
    if (file < 0) return false;
    int exists = maFileExists(file);
    maFileClose(file);
    return 1 == exists;
}

/**
 * Converts a list to a string representation
 * of a JavaScript array.
 *
 * @param   The list to convert
 * @return  The String with the JavaScript representation
 */
String Live::listToJavaScriptArray(List<String> list) {
    if (list.size() == 0) {
        return "[]";
    }
    List<String>::ListIterator itr = list.begin();
    String result = "['" + itr.next() + "',";
    while(itr.hasNext()) result += "'" + itr.next() + "',";
    result[result.size()-1] = ']';
    return result;
}

/**
 * Initializes the Live app.
 */
void Live::initialize() {
    createUI();
    extractFileSystem();
    enableWebViewMessages();

    // Initialize the message handler. All messages from
    // JavaScript are routed through this handler.
    mMessageHandler->initialize(this);
}

/**
 * Creates the main UI elements.
 */
void Live::createUI() {
    // Create and configure the WebView.
    mWebView = new WebView();
    mWebView -> fillSpaceHorizontally();
    mWebView -> fillSpaceVertically();

    // Layouts.
    mVerticalLayout = new VerticalLayout();
    mHorizontalLayout = new HorizontalLayout();
    mHorizontalLayout -> wrapContentVertically();

    mUrlField = new EditBox();
    mUrlField -> fillSpaceHorizontally();

    mReloadButton = new Button();
    mReloadButton->setText("Reload");
    mReloadButton->addButtonListener(this);
    //mReloadButton->setHeight(mUrlField->getHeight());

    mAboutIcon = new ImageButton();
    mAboutIcon->addButtonListener(this);
    mAboutIcon->setBackgroundImage(INFO_ICON);
    mAboutIcon->setSize(mReloadButton->getHeight(),mReloadButton->getHeight());
    //mInfoIcon->setScaleMode(IMAGE_SCALE_PRESERVE_ASPECT);

    mVerticalLayout->addChild(mHorizontalLayout);
    mVerticalLayout->addChild(mWebView);

    mHorizontalLayout->addChild(mUrlField);
    mHorizontalLayout->addChild(mReloadButton);
    //mHorizontalLayout->addChild(mAboutIcon);

    // Create and show the screen that holds the WebView.
    mScreen = new Screen();
    mScreen->setMainWidget(mVerticalLayout);
    mScreen->show();
}

/**
 * Display a page in the WebView of this moblet.
 * @param url   URL of the page to open.
 */
void Live::showPage(const MAUtil::String& url) {
    getWebView()->openURL(url);
}

/**
 * React upon a click on the button reload.
 * @param url   URL of page to open.
 */
void Live::buttonClicked(Widget* button) {
    if (button == mReloadButton) {
        String url = mUrlField->getText();
        if (url == DEFAULT_URL) {
            // invalid url
            return;
        }

        // Save the server address on file.
        mFileUtil->writeTextToFile(
                mFileUtil->getLocalPath() + "LastUrl.txt",
                url);

        showPage(url);
        mUrlField->hideKeyboard();

    } else if (button == mAboutIcon) {
        //Show the info screen
        maMessageBox("MoSync Live", "Load web pages live via jsbin.com");
    }
}

/**
 * Initialize the Wormhole JS library. Should be
 * called after the page has been displayed.
 *
 * @param webViewHandle
 */
void Live::openWormhole(MAHandle webViewHandle) {
    // sendDeviceScreenSizeToJavaScript();
    // sendWebViewHandleToJavaScript();
    mMessageHandler->openWormhole(webViewHandle, this);
    // mWebView->callJS("alert('socket:'+socket);");
}
