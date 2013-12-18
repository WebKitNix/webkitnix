#include "config.h"

#include "PlatformWebView.h"
#include "PlatformUtilities.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKPreferencesPrivate.h>
#include <WebKit2/WKView.h>

namespace TestWebKitAPI {

static bool initialized = false;
static bool terminated = false;
static bool changedTitle = false;
static bool initializedShared = false;
static bool terminatedShared = false;
static bool changedTitleShared = false;

static void didReceiveMessageFromInjectedBundle(WKContextRef, WKStringRef messageName, WKTypeRef, const void*)
{
    if (WKStringIsEqualToUTF8CString(messageName, "WebWorkerInitMessage"))
        initialized = true;
    else if(WKStringIsEqualToUTF8CString(messageName, "WebWorkerTerminateMessage"))
        terminated = true;
    else if (WKStringIsEqualToUTF8CString(messageName, "WebWorkerSharedInitMessage"))
        initializedShared = true;
    else if(WKStringIsEqualToUTF8CString(messageName, "WebWorkerSharedTerminateMessage"))
        terminatedShared = true;
}

static void didReceiveTitleForFrame(WKPageRef, WKStringRef title, WKFrameRef, WKTypeRef, const void *)
{
    if (WKStringIsEqualToUTF8CString(title, "WebWorkerTitle"))
        changedTitle = true;
    else if (WKStringIsEqualToUTF8CString(title, "WebWorkerSharedTitle"))
        changedTitleShared = true;
}

TEST(WebKit2, WebWorker)
{
    // This test cover the binding between functions of JavaScript Workers and
    // functions defined at a previously loaded bundle. It verifies the two
    // types of Workers, dedicated and shared.

    WKRetainPtr<WKContextRef> context = adoptWK(Util::createContextForInjectedBundleTest("WebWorkerTest"));
    PlatformWebView view(context.get());

    WKPageLoaderClientV3 pageLoaderClient;
    memset(&pageLoaderClient, 0, sizeof(pageLoaderClient));
    pageLoaderClient.base.version = 3;
    pageLoaderClient.didReceiveTitleForFrame = didReceiveTitleForFrame;
    WKPageSetPageLoaderClient(WKViewGetPage(view.platformView()), &pageLoaderClient.base);

    WKContextInjectedBundleClientV1 injectedBundleClient;
    memset(&injectedBundleClient, 0, sizeof(injectedBundleClient));
    injectedBundleClient.base.version = 1;
    injectedBundleClient.didReceiveMessageFromInjectedBundle = didReceiveMessageFromInjectedBundle;
    WKContextSetInjectedBundleClient(context.get(), &injectedBundleClient.base);

    WKPageGroupRef pageGroup = WKPageGetPageGroup(WKViewGetPage(view.platformView()));
    WKPreferencesRef preferences = WKPageGroupGetPreferences(pageGroup);
    WKPreferencesSetFileAccessFromFileURLsAllowed(preferences, true);

    WKRetainPtr<WKURLRef> url = adoptWK(Util::createURLForResource("../WebKit2/WebWorkerTest", "html"));
    WKPageLoadURL(WKViewGetPage(view.platformView()), url.get());
    Util::run(&initialized);
    Util::run(&terminated);
    Util::run(&changedTitle);

    url = adoptWK(Util::createURLForResource("../WebKit2/WebWorkerSharedTest", "html"));
    WKPageLoadURL(WKViewGetPage(view.platformView()), url.get());
    Util::run(&initializedShared);
    Util::run(&terminatedShared);
    Util::run(&changedTitleShared);
}

} // TestWebKitAPI
