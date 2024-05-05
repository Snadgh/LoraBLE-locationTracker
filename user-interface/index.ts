/**
 * @license
 * Copyright 2019 Google LLC. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

// Note: This example requires that you consent to location sharing when
// prompted by your browser. If you see the error "The Geolocation service
// failed.", it means you probably did not give permission for the browser to
// locate you.
let map: google.maps.Map, infoWindow: google.maps.InfoWindow;

function initMap(): void {
  map = new google.maps.Map(document.getElementById("map") as HTMLElement, {
    center: { lat: -34.397, lng: 150.644 },
    zoom: 6,
  });
  infoWindow = new google.maps.InfoWindow();

  const locationButton = document.createElement("button");
  const switchModeButton =document.createElement("switchButton");
 const geolocationButton=document.createElement("geoButton");

  locationButton.textContent = "Pan to Device Location";
  switchModeButton.textContent ="Switch to BLE Mode";
  geolocationButton.textContent="Your Position";



  //MQ== is 1, MA== is 0
  const postData = { 
    "payload_raw": "MQ==", //transposes the BleMode boolean value
    "port": 1,
    "confirmed": false
  };
  locationButton.classList.add("custom-map-control-button");


  switchModeButton.classList.add("custom-map-control-button");

  geolocationButton.classList.add("custom-map-control-button");
  map.controls[google.maps.ControlPosition.TOP_CENTER].push(locationButton);
  map.controls[google.maps.ControlPosition.TOP_CENTER].push(geolocationButton);
  
  map.controls[google.maps.ControlPosition.BOTTOM_CENTER].push(switchModeButton);

  switchModeButton.addEventListener('click', async () => {
    const url = 'https://console.helium.com/api/v1/down/eddd4bb5-861b-4f4e-8a22-e5b04119076b/UyAGyEROpKm_4oeLx6sI35R6yysDiV5Q';

  
    try {
      const response = await fetch(url, {
        method: 'POST', 
        headers: {
          'Content-Type': 'application/json',
          // 'Access-Control-Allow-Origin': '*', // to respect CORS policy
          //'Access-Control-Allow-Methods':'DELETE, POST, GET, OPTIONS',
          //'Access-Control-Allow-Headers':'Content-Type, Authorization, X-Requested-With'
        },
        body: JSON.stringify(postData) 
      });
      const status = await response.status;
      console.log(status);

      if(postData.payload_raw=="MQ=="){
        switchModeButton.textContent="Switch to Wifi";
        postData.payload_raw="MA==";
      } else{
        switchModeButton.textContent="Switch to BLE";
        postData.payload_raw="MQ==";
      }
      
    } catch (error) {
      console.error('Error:', error);
    }
  });



  locationButton.addEventListener("click", async () => {

    try {
      const coordinates = await fetchCoordinates();
      console.log('Coordinates:', coordinates);
      const pos = {
        lat: coordinates.lat,
        lng: coordinates.lng,
      };
      infoWindow.setPosition(pos);
      infoWindow.setContent("Location found.");
      infoWindow.open(map);
      map.setCenter(pos);
      // Now you can use the 'coordinates' variable wherever you need in your script
  } catch (error) {
      console.error('Error fetching coordinates:', error);
  }

  });

geolocationButton.addEventListener("click",async ()=>{

      // Try HTML5 geolocation.
      if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(
          (position: GeolocationPosition) => {
            const pos = {
              lat: position.coords.latitude,
              lng: position.coords.longitude,
            };
  
            infoWindow.setPosition(pos);
            infoWindow.setContent("Location found.");
            infoWindow.open(map);
            map.setCenter(pos);
          },
          () => {
            handleLocationError(true, infoWindow, map.getCenter()!);
          }
        );
      } else {
        // Browser doesn't support Geolocation
        handleLocationError(false, infoWindow, map.getCenter()!);
      }
});

}

function handleLocationError(
  browserHasGeolocation: boolean,
  infoWindow: google.maps.InfoWindow,
  pos: google.maps.LatLng
) {
  infoWindow.setPosition(pos);
  infoWindow.setContent(
    browserHasGeolocation
      ? "Error: The Geolocation service failed."
      : "Error: Your browser doesn't support geolocation."
  );
  infoWindow.open(map);
}
async function fetchCoordinates(): Promise<{ lat: number, lng: number, batteryPercentage:number }> {
  const response = await fetch('https://us-central1-locationtracker-6aa67.cloudfunctions.net/get_latest_location_and_battery_from_db'); // Replace 'https://api.example.com/coordinates' with your API endpoint
  if (!response.ok) {
      throw new Error('Failed to fetch coordinates');
  }
  const data = await response.json();
  return data; // Assuming the response is in JSON format and contains latitude and longitude properties + battery
}

declare global {
  interface Window {
    initMap: () => void;
  }
}
window.initMap = initMap;
export {};
