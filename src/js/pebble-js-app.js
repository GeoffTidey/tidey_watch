function HTTPGET(url) {
  var req = new XMLHttpRequest();
  req.open("GET", url, false);
  req.send(null);
  return req.responseText;
}

//------LOCATION------
var getLocation = function() {
  //Get Location
  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  getWeatherData(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "city":"Loc Unavailable",
    "temperature":"N/A"
  });
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 };

//------Utility-------
function evenRound(num, decimalPlaces) {
    var d = decimalPlaces || 0;
    var m = Math.pow(10, d);
    var n = +(d ? num * m : num).toFixed(8); // Avoid rounding errors
    var i = Math.floor(n), f = n - i;
    var e = 1e-8; // Allow for rounding errors in f
    var r = (f > 0.5 - e && f < 0.5 + e) ?
                ((i % 2 == 0) ? i : i + 1) : Math.round(n);
    return d ? r / m : r;
}

//------WEATHER------
var api_key;

var getWeatherData = function(latitude, longitude) {
  // Get weather info
  // use private server, so not to share secret key, if .pbw is public
  var url = "https://api.forecast.io/forecast/" + api_key + "/" + latitude + "," + longitude + "?units=uk&exclude=[currently,hourly,daily,alerts,flags]"
  // console.log("calling: " + url)

  var response = HTTPGET(url);

  // Convert to JSON
  var json = JSON.parse(response);

  // Extract the data
  var apparentTemperature = evenRound(json.currently.apparentTemperature);
  var hourFrom            = json.minutely.data[0].time;
  var hourSummary         = json.minutely.summary;

  var windSpeed           = evenRound(json.currently.windSpeed).toString();
  var windBearing         = json.currently.windBearing;

  // Construct a key-value dictionary
  var dict = { 0: apparentTemperature, 1: hourFrom, 2: hourSummary,
               3: windSpeed, 4: windBearing };

  // Send data to watch for display
  Pebble.sendAppMessage(dict, function(e) {
    // console.log("success");
    }, function(e) {
      // console.log("fail");
  });
};

//------MAIN------
Pebble.addEventListener("appmessage",
  function(e) {
    // Watch wants new data!
    api_key = e.payload.meh;
    getLocation();
  }
);
