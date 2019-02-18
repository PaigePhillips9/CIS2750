****************************************************
Paige Phillips		             0956405
CIS2750	   	   	   	  Assignment 3
Mar 19th 2018		       phillipp@.uoguelph.c
****************************************************


************************
Program Description
**************************
 This program displays information about family trees and generations.
 It has a user interface which displays information about the GEDCOM
 files stored on the server. It uses the GEDCOM parser created in
 previous assignments to get information from files. The program also
 uses SQL databases to sort and display the information.Genealogy App 
 can show all individuals in a file, the files stored in server, it 
 can upload a file, create a new GEDCOM file, add individuals to an 
 existing file and get the descendants or ancestors of an individual. 
 There is a status box to display the success or failure of
 each of theses operations.

 The additional SQL queries I have included should display all individuals
 with a specific last name and of a specific gender, display the average
 number of individuals in a files and display the list of individuals
 with information about the source and submitter of the file.

*************************
Known Bugs
*************************
Store all files is very slow due to the nested for loops with sql queries inside.
Due to the asynchonousity of the ajax calls, sometimes things happen in the wrong order.
