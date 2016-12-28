var mpmetrics;
try {
	mpmetrics = new MixpanelLib("6415fe3ccaa5e9b361139cd9a1e50f9e");
} catch(err) {
	var null_fn = function () {};
	mpmetrics = { track: null_fn, track_funnel: null_fn, register: null_fn, register_once: null_fn };
}
var jQuery;
var analytics_id;
(function(){
	var cookie_match;
	var c = document.cookie;
	if (c && (cookie_match = /(?:^|;)analyticsId=([a-fA-F0-9]*)(?:$|;)/.exec(c)))
	{
		analytics_id = cookie_match[1];
	}
	else
	{
		analytics_id = "";
		for (var i = 0; i < 8; ++i)
		{
			analytics_id += Math.floor(Math.random() * 65536).toString(16);
		}
	}
	var ex = new Date();
	ex.setTime(ex.getTime() + (365 * 24 * 60 * 60 * 1000));
	var newcookie = "analyticsId=" + analytics_id + ";path=/;domain=philjordan.eu;expires=" + ex.toUTCString();
	document.cookie = newcookie;
	mpmetrics.identify(analytics_id);
	mpmetrics.track("pageview", { "page": window.location.toString(), "referrer": document.referrer, "referrer-domain": ((/^https?:\/\/([^\/]+)\//).exec(document.referrer) || [])[1], "browser":window.navigator.userAgent });
	
	if (!jQuery) return;
	var tracked_links = jQuery("a.track");
	tracked_links.click(function(ev){
		var url = $(this).attr("href");
		function followLink() {
			window.location = url;
		}
		mpmetrics.track("click-link", { "url": url, "onpage": window.location.toString() }, followLink);
		setTimeout(500, followLink);
		return false;
	});
})();
