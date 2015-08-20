Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS Ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://680d557c.ngrok.com/';

  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));

  console.log('Configuration page returned: ' + JSON.stringify(configData));

  if (configData.showWeather) {
    Pebble.sendAppMessage({
      showWeather: configData.showWeather ? 1 : 0,
      useCelsius: configData.useCelsius ? 1 : 0
    }, function() {
      console.log('Send successful!');
    }, function() {
      console.log('Send failed!');
    });
  }
});