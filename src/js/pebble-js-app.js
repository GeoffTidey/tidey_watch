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
  var url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + latitude + "&lon=" + longitude + "&cnt=10&mode=json"
  console.log("calling: " + url)
  var response = HTTPGET(url);

  //Convert to JSON
  var json = JSON.parse(response);

  //Extract the data
  var weatherDatetime = parseInt(json.list[1].dt);
  var temperature     = Math.round(json.list[1].main.temp - 273.15);
  var weatherDesc     = json.list[1].weather[0].description;
  var location        = json.city.name;

  //Console output to check all is working.
  var date = new Date(weatherDatetime * 1000);
  console.log("It is " + temperature + " degrees in " + location + " in " + date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds());

  //Construct a key-value dictionary
  var dict = { 0: temperature, 1: location, 2: weatherDesc, 3: weatherDatetime };

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