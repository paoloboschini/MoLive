/* Copyright (C) 2013 Paolo Boschini */

#ifndef LIVE_H
#define LIVE_H

#include <Wormhole/HybridMoblet.h>
#include <NativeUI/VerticalLayout.h>
#include <NativeUI/HorizontalLayout.h>
#include <NativeUI/EditBox.h>
#include <NativeUI/Button.h>
#include <NativeUI/ButtonListener.h>
#include <NativeUI/ImageButton.h>
#include <MAUtil/String.h>
#include <MAUtil/List.h>

#include "MAHeaders.h" // Defines INFO_ICON
#include "Live.h"

// WebView widget
using namespace NativeUI;

// Wormhole library
using namespace Wormhole;

// Used for String
using namespace MAUtil;

/**
 * This class provides a simple WebView that loads
 * the web page at the specified URL. It saves the
 * most recent entered URL.
 */
class Live : public HybridMoblet, public ButtonListener {

private:

    VerticalLayout *mVerticalLayout;
    HorizontalLayout *mHorizontalLayout;
    Button *mReloadButton;
    ImageButton *mAboutIcon;
    EditBox *mUrlField;
    const String DEFAULT_URL;

public:

    /**
     * Constructor.
     */
    Live();

    /**
     * Called from JavaScript to restore the main WebView.
     * This method is useful to clean up Native UI elements
     * that reside on top of the main WebView.
     *
     * @param message The stream sent from JavaScript.
     */
    void restoreWebView(Wormhole::MessageStream& stream);

    /**
     * Called from JavaScript to download a resource.
     *
     * @param stream The stream sent from JavaScript.
     */
    void downloadResource(Wormhole::MessageStream& stream);

    /**
     * Called from JavaScript to get the resources on this
     * device. Resources reside in the resource folder.
     *
     * @param stream The stream sent from JavaScript.
     */
    void getListResources(Wormhole::MessageStream& stream);

    /**
     * Gets a file list contained in the given directory.
     *
     * @param path  The path where to look for the file list.
     * @return      The list of files in the given path.
     */
    List<String> Live::getFileList(const char* path);

    /**
     * Creates a file or a directory.
     *
     * @param path  The file or directory to create
     * @return      True on success, false on fail.
     */
    bool Live::fileCreate(const String& path);

    /**
     * Check that a file exists.
     *
     * @param path  The file or directory to check.
     * @return      True if path exists, false if it does not exist.
     */
    bool fileExists(const String& path);

    /**
     * Converts a list to a string representation
     * of a JavaScript array.
     *
     * @param   The list to convert
     * @return  The String with the JavaScript representation
     */
    String listToJavaScriptArray(List<String>);

    /**
     * Initializes the Live app.
     */
    void initialize();

    /**
     * Creates the main UI elements.
     */
    void createUI();

    /**
     * Display a page in the WebView of this moblet.
     * @param url   URL of the page to open.
     */
    void showPage(const MAUtil::String& url);

    /**
     * React upon a click on the button reload.
     * @param url   URL of page to open.
     */
    void buttonClicked(Widget* button);

    /**
     * Initialize the Wormhole JS library. Should be
     * called after the page has been displayed.
     *
     * @param webViewHandle
     */
    void openWormhole(MAHandle webViewHandle);

};

#endif
