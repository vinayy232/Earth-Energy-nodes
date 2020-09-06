# Earth Energy 


Guidelines to use codes:
Master code:
0x026 - ID of Ignition node
0x036 - Id of Aux node

Ignition node - complete msg0 and only starting two bits are useful, rest bit can be used for future purpose.
	Only 5 Inputs and 4 output can be Interfaced
	Bits Defination:
		11- Manual control
		10 - Load ON
		01 - Load OFF
	5 bits which are printed on the Serial monitor shows the status of the switch depending upon Manual and Auto.
Note: If Auto is saying that the Load must be ON, then even if you try to change the Status of that load with switch ,the status wont change and the bits which is reflected on serial monitor will be as per the decision taken by master.
i.e if master says 2nd load to be On and rest to be OFF - then bits on serial will be = 10111

Aux Node - complete msg0,msg1 and starting 4 bits of msg2 is useful,rest bit can be used for future purpose.
	msg2- 0th and 1st bit is for Load 9, and 2nd and 3rd bit should always be kept 11 , it acts as a key for Aux node, If master want to turn off 	all Load simply give the msg2 as 2nd and 3rd bit with anything apart from 11.

	Only 9 Inputs and 9 output can be Interfaced
	Bits Defination:
		11- Manual control
		10 - Load ON
		01 - Load OFF	
	9 bits which are printed on the Serial monitor shows the status of the switch depending upon Manual and Auto.




