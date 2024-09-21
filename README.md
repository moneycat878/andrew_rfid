# RFID Door Lock Project

This is a simple adurino project to read programmed cards from memory and unlock a door tumbler. 

## Nathan M Contributions
Updates were made to the code to include additional features. These features are:
- Dynamically resizable memory array of card IDs
- Master add key
- Master delete key
- Buzzer for sound

The c array that stores the list of working RFID cards is resizeable and can be changed at runtime with the scan of the master add or delete card. To add or delete an entry from the array, scan the appropriate master add/delete card followed by the card to add/delete. 

This will play a buzzer sound to indicate the system has correctly registered the RFID card.

**RFID cards are not yet saved in non-volatile memory. Powering the adurino will reset the stored array to factory programming**

### Future Updates
Future updates to the code will incldue:
- Persitance of stored RFID card list after power off (saving array in non-volatile memory)
- Network connection for remote monitoring and unlocking
- Failsafe option in case of power loss to the adurino
