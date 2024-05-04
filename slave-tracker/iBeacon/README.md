# LoraBLE-locationTracker
34346 Networking Technologies and Application Development for IoT - Group 4

## iBeacon


We now specify the length (0x26) of the frame, and the type of data (0xFF)(manufacturer data) that is encompassed in the advertising packet. Those are precise values that must be respected to comply with IBeacon protocol data unit (PDU) specification.

In our case, we used a UUID generator\footnote{\url{https://www.uuidgenerator.net/}} to be sure to use a unique identifier, randomly generated: %e6e9a717-9cc0-4485-8fb1-941f05273c8d. Shorter UUID exists when you propose common services such as Heart Monitoring in smart watches, but we have a specific use case so we must create a new one.

For the company ID, we legally do not have the right to use the manufacturer code of Apple Inc: 0x004c. Therefore we decided to use the reserved one "0xFFFF". It's simply a variable to indicate who is the manufacturer behind the technology.
