/* Copyright (C) 2013 Paolo Boschini */

#include "ResourceDownloader.h"

/**
 * Constructor.
 *
 * @param filename      The filename for saving the resource.
 * @param callbackId    The JavaScript callback to call on download complete
 * @param webView       The WebView used to call JavaScript
 */
ResourceDownloader::ResourceDownloader(String filename, String callbackId, WebView *webView) :
    mFilename(filename), mCallbackId(callbackId), mWebView(webView) {
}

/**
 * Called when download is complete.
 * @param text Contains the resource data
 */
void ResourceDownloader::onDownloadComplete(MAHandle data) {

    FileUtil fileUtil;

    if (data == NULL) {
        printf("Resource data is NULL, could not download resource");
        // execute the callback, inform JS that the resource was not saved
        char script[512];
        String s = script;
        sprintf(script, "mosync.bridge.reply(%s, '%s')", mCallbackId.c_str(), "Resource not saved");
        mWebView->callJS(script);
    } else {
        printf("Resource data is valid, the resource was downloaded");

        fileUtil.writeDataToFile(fileUtil.getLocalPath() + "resources/" + mFilename.c_str(), data);

        // execute the callback, inform JS that the resource was saved
        char script[512];
        sprintf(script, "mosync.bridge.reply(%s, '%s')", mCallbackId.c_str(), "Resource saved");
        mWebView->callJS(script);
    }

    // Delete me!
    delete this;
}
