//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

enum MCFAMessageTypes 
{ 
    	MCFA_ASFREQ 	= 10;
    	MCFA_ASFREP		= 20;
    	MCFA_RERM 		= 30;
    	MCFA_JREQ 		= 40;
    	MCFA_JREP 		= 50;
    	MCFA_CHSEL	 	= 60;
    	MCFA_LREQ 		= 70;
    	MCFA_MOBINFO	= 80;
}; 
    
packet ClusterMCFA  {
	int msgtype enum(MCFAMessageTypes);
	int headId;
	int originId;
	double ERM;
	double Speed;
	double Direction;
};