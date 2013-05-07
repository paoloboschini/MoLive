/* Copyright (C) 2013 Paolo Boschini */

#ifndef RESOURCE_DOWNLOADER_H
#define RESOURCE_DOWNLOADER_H

#include <Wormhole/HybridMoblet.h>
#include <MAUtil/String.h>
#include <Wormhole/HighLevelBinaryDownloader.h>

// WebView widget
using namespace NativeUI;

// Wormhole library
using namespace Wormhole;

// Used for String
using namespace MAUtil;

/**
 * Helper class for downloading a list of resources from a server.
 * This downloader will delete itself upon completion.
 */
class ResourceDownloader : public HighLevelBinaryDownloader {

private:

    String mFilename;
    String mCallbackId;
    WebView *mWebView;

public:

    /**
     * Constructor.
     *
     * @param filename      The filename for saving the resource.
     * @param callbackId    The JavaScript callback to call on download complete
     * @param webView       The WebView used to call JavaScript
     */
    ResourceDownloader(String filename, String callbackId, WebView *webView);

    /**
     * Called when download is complete.
     * @param text Contains the resource data
     */
    virtual void onDownloadComplete(MAHandle data);

};

#endif
