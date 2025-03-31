struct Station {   //to represent each gas station these are the main properties 
   int stationID;
  string name;
  float price;
};

struct  Connection_with_other       // to represent the distance with destination gas station 
{
	int destinationID;
	double distance;
	
};