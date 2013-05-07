#ifndef RESOURCE_DOWNLOADER
#define RESOURCE_DOWNLOADER

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
 * Helper class for downloading a list of photos from a server.
 * This downloader will delete itself upon completion.
 */
class ResourceDownloader : public HighLevelBinaryDownloader {

private:
    String mFilename;
    String mCallbackId;
    WebView *mWebView;

public:
    ResourceDownloader(String filename, String callbackId, WebView *webView);

    virtual ~ResourceDownloader();

    /**
     * Called when download is complete.
     * @param text Contains JSON with list of image urls, NULL on error.
     */
    virtual void onDownloadComplete(MAHandle data);

};

#endif
