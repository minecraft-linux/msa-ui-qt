(function() {
    var cbs = [];
    var conn = new QWebChannel(qt.webChannelTransport, function() {
        for (var i = 0; i < cbs.length; i++)
            cbs[i](conn);
        cbs = null;
    });
    function withConn(cb) {
        if (cbs !== null)
            cbs.push(cb);
        else
            cb(conn);
    }

    if (!window.webkit)
        window.webkit = {};
    if (!window.webkit.messageHandlers)
        window.webkit.messageHandlers = {};
    var external = webkit.messageHandlers;

    external.Property = {};
    external.Property.postMessage = function(props) {
        props = JSON.parse(props);
        for (var k in props) {
            if (!props.hasOwnProperty(k))
                continue;
            (function(name, value) {
                withConn(function (conn) {
                    conn.objects.loginWindow.setProperty(name, value);
                });
            })(k, props[k]);
        }
    };
    external.FinalNext = {};
    external.FinalNext.postMessage = function() {
        withConn(function(conn) {
            conn.objects.loginWindow.onFinalNext();
        });
    };
    external.FinalBack = {};
    external.FinalBack.postMessage = function() {
        withConn(function(conn) {
            conn.objects.loginWindow.onFinalBack();
        });
    };
})();