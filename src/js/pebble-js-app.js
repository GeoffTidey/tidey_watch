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

//------WEATHER------
var api_key;

var getWeatherData = function(latitude, longitude) {
  // Get weather info
  var url = "https://api.forecast.io/forecast/" + api_key + "/" + latitude + "," + longitude + "?units=uk&exclude=[currently,hourly,daily,alerts,flags]"
  // console.log("calling: " + url)

  var response = HTTPGET(url);

  // Convert to JSON
  var json = JSON.parse(response);

  // Extract the data
  var apparentTemperature = json.currently.apparentTemperature;
  var hourFrom            = json.minutely.data[0].time;
  var hourSummary         = json.minutely.summary;

  // Construct a key-value dictionary
  var dict = { 0: apparentTemperature, 1: hourFrom, 2: hourSummary };

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
