/**
 * @ Author: Oscar Theilvig Strømsborg
 * @ Create Time: 2024-05-04 15:56:15
 * @ Modified by: Oscar Theilvig Strømsborg
 * @ Modified time: 2024-05-04 16:04:15
 * @ Description: This function decodes a payload to extract MAC addresses and battery percentage from a given array of bytes. It supports up to 8 MAC addresses.
 */
function Decoder(bytes, port) {
  // Calculate the number of MAC addresses by adjusting for the battery percentage byte and dividing by the byte length of a MAC address.
  var numMacAddresses = (bytes.length - 1) / 6;
  
  // Check if the computed number of MAC addresses is a whole number within the valid range (1 to 8).
  if (numMacAddresses < 1 || numMacAddresses > 8 || Math.floor(numMacAddresses) !== numMacAddresses) {
    // Return an error message if the number of MAC addresses is invalid.
    return {
      error: "Invalid number of MAC addresses"
    };
  }

  // Initialize an array to store decoded MAC addresses.
  var macAddresses = [];
  // Loop through the bytes array in steps of 6 (size of a MAC address) to extract and format each MAC address.
  for (var i = 0; i < numMacAddresses * 6; i += 6) {
    var macAddress = bytes.slice(i, i + 6)
      .map(function(byte) {
        // Convert each byte into a two-digit hexadecimal string.
        return ('0' + (byte & 0xFF).toString(16)).slice(-2);
      })
      .join(':'); // Join all parts of the MAC address with colons.
    macAddresses.push(macAddress.toUpperCase()); // Store the MAC address in uppercase.
  }

  // Decode the battery percentage from the last byte of the input array.
  var batteryPercentage = bytes[bytes.length - 1];
  // Convert the battery byte to a percentage.
  batteryPercentage = Math.round((batteryPercentage / 255) * 100);

  // Construct the result object by assigning each MAC address and the battery percentage to keys.
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
