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

var getWeatherData = function(latitude, longitude) {

  //Get weather info
  // var url = "https://api.forecast.io/forecast/" + api_key + "/" + + latitude + "," + longitude + "?units=uk&exclude=[currently,daily,alerts,flags]"
  var url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + latitude + "&lon=" + longitude + "&cnt=10&mode=json"
  console.log("calling: " + url)
  var response = HTTPGET(url);

  //Convert to JSON
  var json = JSON.parse(response);

  var epochTimeNow = parseInt((new Date).getTime() / 1000);
  var offset = 0;
  console.log("now epoch: " + epochTimeNow)
  for (var i = 0; i < json.list.length; i++) {
    console.log(i + " list: " + json.list[i].dt)
    if (json.list[i].dt >= epochTimeNow) {
      offset = i;
      console.log(i + " greater epoch: " + json.list[i].dt)
      break;
    }
  }

  //Extract the data
  var weatherDatetime = parseInt(json.list[offset].dt);
  var weatherDateDiff = weatherDatetime - epochTimeNow;
  var temperature     = Math.round(json.list[offset].main.temp - 273.15);
  var weatherDesc     = json.list[offset].weather[0].description;
  var location        = json.city.name;

  //Console output to check all is working.
  console.log("It is " + temperature + " degrees in " + location + " => " + weatherDateDiff + "seconds. " + weatherDesc);

  //Construct a key-value dictionary
  var dict = { 0: temperature, 1: location, 2: weatherDesc, 3: weatherDateDiff };

  //Send data to watch for display
  Pebble.sendAppMessage(dict, function(e) {
    console.log("success");
    }, function(e) {
      console.log("fail");
  });
};


//------MAIN------

Pebble.addEventListener("ready",
  function(e) {
    //App is ready to receive JS messages
    getLocation();
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
    //Watch wants new data!
    getLocation();
  }
);