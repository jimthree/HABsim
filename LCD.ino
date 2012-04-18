void LCDFlight()
{
 //Lat & lon
 if(CurLat>0&&CurLat<10) lcd.setCursor(2, 0); //eg. 2.345  
 if(CurLat>10) lcd.setCursor(1, 0);  //eg. 23.456
 if(CurLat<0&&CurLat>-10) lcd.setCursor(1, 0);  //eg. -2.345
 if(CurLat<-10) lcd.setCursor(0, 0);  //eg. -23.456
 lcd.print(CurLat,3);
 
 if(CurLon>0&&CurLon<10) lcd.setCursor(2, 1); //eg. 2.345  
 if(CurLon>10) lcd.setCursor(1, 1);  //eg. 23.456
 if(CurLon<0&&CurLon>-10) lcd.setCursor(1, 1);  //eg. -2.345
 if(CurLon<-10) lcd.setCursor(0, 1);  //eg. -23.456
 lcd.print(CurLon,3);
 

 //Current Alt
 if(CurAlt>10000) 
 {
   lcd.setCursor(8, 0);  
 }

 if(CurAlt<10000) 
 {
   lcd.setCursor(8, 0);  
   lcd.print(" ");
   lcd.setCursor(9, 0);  
 }

 if(CurAlt<1000) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("  ");
   lcd.setCursor(10, 0);  
 }

 if(CurAlt<100) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("   ");
   lcd.setCursor(10, 0);  
 }

 if(CurAlt<10) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("    ");
   lcd.setCursor(11, 0);  
 }

 if(CurAlt<1) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("     ");
   lcd.setCursor(12, 0);  
 }

 lcd.print(CurAlt,0);



// Current Speed ////////////BUG HERE////////////

 if(CurSpeed>9) 
 {
   lcd.setCursor(14, 0);  
 }
 if(CurSpeed<10) 
 {
   lcd.setCursor(14, 0);  
   lcd.print(" ");
   lcd.setCursor(15, 0);  
 }
 lcd.print(CurSpeed,0);





// Current Pressure
 
 if(CurKPA>100) 
 {
   lcd.setCursor(9, 1);
 }
 
 if(CurKPA<100)
 { 
   lcd.setCursor(9, 1);
   lcd.print(" ");
   lcd.setCursor(10, 1);
 }
 
 if(CurKPA<10)  
 {
   lcd.setCursor(9, 1);
   lcd.print("  ");
   lcd.setCursor(11, 1);
 }
 lcd.print(CurKPA,2);



// Status Icon

 lcd.setCursor(15, 1);

   if(Status==0) lcd.print((char)0);
   if(Status==1) lcd.print((char)1);
   if(Status==2) lcd.print((char)2);



// graph
float graph_pos = fscale(0,35000,0,9,CurAlt,0);


 lcd.setCursor(7, 0);
 lcd.print((char)3);
 lcd.setCursor(7, 1);
 lcd.print((char)3);


 lcd.setCursor(7, 1);
 if(int(graph_pos)==0) lcd.print((char)3);
 if(int(graph_pos)==1) lcd.print((char)4);
 if(int(graph_pos)==2) lcd.print((char)5);
 if(int(graph_pos)==3) lcd.print((char)6);
 if(int(graph_pos)==4) lcd.print((char)7);

 lcd.setCursor(7, 0);
 if(int(graph_pos)==5) lcd.print((char)4);
 if(int(graph_pos)==6) lcd.print((char)5);
 if(int(graph_pos)==7) lcd.print((char)6);
 if(int(graph_pos)==8) lcd.print((char)7);

}











