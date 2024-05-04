function Decoder(bytes, port) {
  // Calculate the number of MAC addresses based on the payload length
  // Subtract 1 for the battery percentage byte, then divide by 6 (bytes per MAC address)
  var numMacAddresses = (bytes.length - 1) / 6;
  
  // Check if the number of MAC addresses is within the valid range (1 to 8)
  if (numMacAddresses < 1 || numMacAddresses > 8 || Math.floor(numMacAddresses) !== numMacAddresses) {
    return {
      error: "Invalid number of MAC addresses"
    };
  }

  // Decode the MAC addresses
  var macAddresses = [];
  // json body to add mac addresses to
  for (var i = 0; i < numMacAddresses * 6; i += 6) {
    var macAddress = bytes.slice(i, i + 6)
      .map(function(byte) {
        // Convert each byte to a 2-digit hexadecimal number
        return ('0' + (byte & 0xFF).toString(16)).slice(-2);
      })
      .join(':');
    macAddresses.push(macAddress.toUpperCase());
  }

  // Decode the battery percentage
  var batteryPercentage = bytes[bytes.length - 1]; // Assuming the battery byte is the last one
  // map the battery percentage to a value between 0 and 100
  batteryPercentage = Math.round((batteryPercentage / 255) * 100);

  // return as a json with mac1, mac2, etc as keys and eventually the battery percentage
  return {
    "mac1": macAddresses[0],
    "mac2": macAddresses[1],
    "mac3": macAddresses[2],
    "mac4": macAddresses[3],
    "mac5": macAddresses[4],
    "mac6": macAddresses[5],
    "mac7": macAddresses[6],
    "mac8": macAddresses[7],
    "batteryPercentage": batteryPercentage
    
  };
}
