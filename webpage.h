const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Get Location Example</title>
    <script>
        var socket;

        function initWebSocket() {
            socket = new WebSocket("ws://192.168.1.15:8080");

            socket.onopen = function() {
                console.log("WebSocket connection established.");
            };

        socket.onclose = function(event) {
            console.log("WebSocket connection closed: " + event.code);
        };

        socket.onerror = function(error) {
            console.log("WebSocket error: " + error);
        };

        socket.onmessage = function(e) {  
                    console.log("[socket] " + e.data);
                    // document.getElementById("mrdiy_value").innerHTML = e.data;
                    getLocation();
                };
    }

    function getLocation() {
      if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(showPosition);
      } else {
        alert("Geolocation is not supported by this browser.");
      }
    }

    function showPosition(position) {
      var latitude = position.coords.latitude;
      var longitude = position.coords.longitude;

      document.getElementById("latitude").textContent = "Latitude: " + latitude;
      document.getElementById("longitude").textContent = "Longitude: " + longitude;

      var locationData = {
            latitude: latitude,
            longitude: longitude
          };
          console.log("location: " + latitude + ", " + longitude);
          socket.send(JSON.stringify(locationData));
    }
    
  </script>
</head>
<body>
  <h1>Get Location Example</h1>
  <button onclick="initWebSocket()">Connect WebSocket</button>
  <button onclick="getLocation()">Get Location</button>
  <p id="latitude"></p>
  <p id="longitude"></p>
</body>
</html>
)rawliteral";