//Calls the required libraries to parse the text file
import java.io.*;
import java.util.*;

//creates a public class for the text file to be read into
public class readfile {
    //creates a scanner X which will be used to read in the file
	private Scanner x;
    
    //opens the file at a specified location
    public void openFile(){
        try{
           //Sets the scanner x which will input text from a file
        	x = new Scanner(new File("lab01.txt"));// change location depending on file desired
        }
        catch(Exception e){
        	//prints exception if file cannot be found
        	System.out.println("Could not find the file"); 
        }
    }
    
    public void readFile(){
        // while there is still information in the file read the next line
    	while(x.hasNext()){
            String a = x.next();
            //this portion should change when adapting to the rest of the code so that it returns to another function to be dealt with
            System.out.printf("%s\n",a); // prints each portion on its own line
        }
    }
    
    //after completing the file reading it closes the file
    public void closeFile(){
        x.close();
    }
}
