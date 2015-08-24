var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = 'https://api.forecast.io/forecast/6a987aa0f3fdb98e6582163f2c9c324f/' + 
  pos.coords.latitude + ',' + pos.coords.longitude;
  
  console.log("Lat is " + pos.coords.latitude);
  console.log("Lon is " + pos.coords.longitude);

  // Send request to forecast.io
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
    console.log(JSON.parse(responseText));

      var temperature = Math.round(json.currently.apparentTemperature);
      console.log("Temperature is " + temperature);
      
    var temperaturec = Math.round((json.currently.apparentTemperature - 32) * 5/9);
    console.log("Temperature in Celsius is " + temperaturec);

    var conditions = json.minutely.summary;      
    // if (conditions[conditions.length - 1] == '.')  // check for a trailing period
    var conditionsm = conditions.substr(0, conditions.length - 1);  // chop off the last character
    console.log("Conditions are " + conditions);  

      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_TEMPERATURE_IN_C": temperaturec,
        "KEY_CONDITIONS": conditionsm,
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS Ready! Getting weather.');

  getWeather();
});

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received! Updating weather.');

    getWeather();
  }                     
);

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://turnervink.github.io/vacationtime/';

  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));

  console.log('Configuration page returned: ' + JSON.stringify(configData));

  Pebble.sendAppMessage({
    showWeather: configData.showWeather ? 1 : 0,
    useCelsius: configData.useCelsius ? 1 : 0
  }, function() {
    console.log('Send successful!');
  }, function() {
    console.log('Send failed!');
  });
});