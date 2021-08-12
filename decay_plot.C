#include <iostream>
#include <fstream>
#include "TCanvas.h"
#include "TTree.h"
#include "TGraph.h"
#include "TPaveStats.h"
#include "TText.h"
#include "TF1.h"
#include "TStyle.h"
#include "date.h"

using namespace std;

/*
 * This code is used to plot the currents of the RPCs in function of time and fit the curve with a exponencial + constante function.
 * Be in mind that it is to be used when one is interested in analyse how much time is necessary to reduce the RPC currents when it 
 * is subject to a HV condionting + stability scan without irradiate the chamber, so is possible to see the currents decreasing.
 * For more information check the README file on github page.

 * In case of questions:
 * mapse.b@cern.ch
 * mapse.b@hotmail.com
*/

/**
 * This function is used to read the csv file with the data, plot the curve and fit it.
 * @param csvfile Is a string with the csv file name
 * @param chamber Is the chamber name one want to analyse.
 * @param nPoints Is the number of points to be plotted.
 * @param dt Are the dates to be considered (this array must have the same size as the number of points).
 * @param volt Is the desired voltage.
 */

void dataRead(string csvfile, string chamber, int const nPoints, string dt[], double volt){
   
    // Tree for read date from a csc file.
    TTree *tree = new TTree("Data for scan","Data from csv file"); 
    // Method to read the file .csv with two variables.
   
    // Convert the string to char in order to use it on a ttree readfile
    char* csv_ttree;
    string str_obj(csvfile);
    csv_ttree = & str_obj[0];
    // Read the csv file with the ttree
    tree->ReadFile(csv_ttree,"CHAMBER_NAME_CMSSW/C:d1/C:d2/C:d3/C:vmon/D:imon/D",',');
    // Variables to put on the tree branch.
    double current, voltage;
    char chamb[20], date[20];
    // Setting the branch address with those variables.
    tree->SetBranchAddress("CHAMBER_NAME_CMSSW", &chamb);
    tree->SetBranchAddress("d1", &date);
    tree->SetBranchAddress("imon", &current);
    tree->SetBranchAddress("vmon", &voltage);
    // Creation of the arrays current and date for plot.
    const auto n = tree->GetEntries();
    double curr[nPoints];
    double tt[nPoints];
    
    //std::cout << dt[] << std::endl;
            
    // Conversion factor to change from day to seconds
    int const dayTosec = 86400;
    
    tt[0] = 0;
    
    // some auxiliary variables
    std::string auxYear1, auxYear2;
    
    std::string auxMonth1, auxMonth2;
    
    std::string auxDay1, auxDay2;
    // Variables of the two different dates
    int year1, year2, month1, month2, day1, day2;

    // day, month, year.
    Date dt1 = {1, 2, 2000}; 
    Date dt2 = {1, 2, 2000}; 

    // Loop for fill time array.
    // This is used to calculate the difference of days between two dates with the help of date.h
    for (unsigned it = 1; it != nPoints; ++it ){
        // Conversion from string format "yyyy.mm.dd" to interger format yyyy, mm, dd
        auxYear1 = dt[it-1];
        auxYear1.resize(4);
        year1 = std::stoi(auxYear1); // stoi converts the string to integer
        
        auxYear2 = dt[it];
        // Some csv files comes with an annoying "", this remove it.
        if (auxYear2 == "") continue;
        auxYear2.resize(4);
        year2 = std::stoi(auxYear2);
               
        auxMonth1 = dt[it-1];
        auxMonth1 = auxMonth1.substr(5,2); // Substr: the first argumetn is the position, the second is the number of characteres one wants to keep
        month1 = std::stoi(auxMonth1);

        auxMonth2 = dt[it];
        if (auxMonth2 == "") continue;        
        auxMonth2 = auxMonth2.substr(5,2);
        month2 = std::stoi(auxMonth2);
        
        auxDay1 = dt[it-1];
        auxDay1 = auxDay1.substr(8,2);
        day1 = std::stoi(auxDay1);

        auxDay2 = dt[it];
        auxDay2 = auxDay2.substr(8,2);
        day2 = std::stoi(auxDay2);

        // day, month, year.
        dt1 = {day1, month1, year1}; 
        dt2 = {day2, month2, year2};

        // Use the function getDifference from date.h in order to compute the number of days between two dates. 
        tt[it] = (getDifference(dt1, dt2) + tt[it-1]);
        
        
    }
        
    std::string auxDate;
    // Loop for take the data for each date considered
    for (unsigned j = 0; j!=nPoints; ++j){
        // Loop inside the ttree in order to get the data
        for (unsigned i = 0; i != n; ++i){
            tree->GetEntry(i);
            // Choose the desired chamber to plot and fit the data
            if(chamb == chamber){    
                // Choose the desired voltage            
                if(voltage == volt){ 
                    // Take the dates from the ttree and store it on a auxiliary variable
                    auxDate = date;
                    // Remove some problems that sometimes comes from the csv file
                    if (auxDate[0] == '"'){
                        auxDate.erase(0,1); // erase: first argument is the position, second is the quantity to be removed
                    }
                    if (auxDate[20] == '"'){
                        auxDate.erase(19,20);
                    }
                    // Resise it to the patter yyyy.mm.dd
                    auxDate.resize(10);
                    //cout << auxDate << endl; //This is to check if the string is correct
                    // Choose the desired dates                         
                    if (auxDate == dt[j]){
                        // If the all previous if are satisfied we finnaly get the current
                        curr[j] = current;
                        
                    }                   
                }
            }     
        }
    }
    
    // The following loop is a way to see if we have the correct date for current and time.
    //for (unsigned bf = 0; bf != nPoints; ++bf){cout << "current: " << curr[bf]  << " seconds: " << tt[bf] << endl;}

    /////////// Plot and fitting section \\\\\\\\\\\

    // Beautiful canvas
    TCanvas *c1 = new TCanvas("c1","Exponential behavior of current",200,10,700,500);

    gPad->SetGrid();
	// Margins
	c1->SetTopMargin(.08);
	c1->SetRightMargin(0.05);
	c1->SetBottomMargin(0.13);
	c1->SetLeftMargin(0.16);
	// Frame and borders
	c1->SetBorderSize(2);
	c1->SetFrameFillStyle(0);
	c1->SetFrameBorderMode(0);
	c1->SetFrameFillStyle(0);
	c1->SetFrameBorderMode(0);
    
    // Graph with the current and date. 
    TGraph *gr1 = new TGraph(nPoints, tt, curr);
    gr1->Draw("AP");
    
    // Marker style (cross).
    gr1->SetMarkerStyle(47);
    // Marker size (1 - default).
    gr1->SetMarkerSize(1.2);
    // Marker color (kpurple=890).
    gr1->SetMarkerColor(890);
    // Title and axis.
    TString a = "Current for ";
    TString b = " at ";
    int kVolt = volt/1000;
    TString c = std::to_string(kVolt);
    TString d = " kV";
    gr1->SetTitle(";  t (days); I (#muA) ");
        
    // const + exponential function for fitting.
    TF1 *sum = new TF1("sum", "[0]+[1]*TMath::Exp(-[2]*x)" );
    // Guess the parameters (If the fit goes bad you can do it on the fit panel and then improve this parameters)
    sum->SetParameters(-4, 10, -8e-9);
    // Keeping the fit results.
    TFitResultPtr fit = gr1->Fit(sum,"S M");
    
    gStyle->SetOptFit(11);
    gStyle->SetStatX(0.93);
    gStyle->SetStatY(0.90);
    
    TPaveStats *st = (TPaveStats*)fit->FindObject("stats");
    
    gr1->Draw("AP");
    TString cur = "Current in time for ";
    
    // Texts
	TLatex txt; 
	txt.SetNDC();
	txt.SetTextFont(43);
	txt.SetTextSize(20);
	txt.SetTextAlign(31);
	txt.DrawLatex(.95,.93, "CMS Cavern");

    // CMS preliminary
	txt.SetTextSize(30);
	txt.SetTextAlign(13);
	txt.DrawLatex(.15,.97,"#bf{CMS} #scale[0.7]{#it{Preliminary}}");

    // Fitting function
	txt.SetTextSize(20);
	txt.SetTextAlign(31);
	txt.DrawLatex(0.86,0.675,"I(t) = p_{0} + p_{1} e^{-p_{2}t} ");

    // Chamber name
	txt.SetTextSize(20);
	txt.SetTextAlign(31);

    // Write the chamber name on the canvas.
    // const char *chamber_name = chamber.c_str();
    // txt.DrawLatex(0.86,0.82,chamber_name);
	
	c1->Update();
    
    c1->SaveAs(cur + chamber + ".png");  
   // c1->Close();  
    
}

int main(){

    int const nPoints = 4;
    string csvfile = "Data_converted_2020.csv";
   
    // Date good for 07
    //std::string dats[nPoints] = {"2020-10-06","2020-10-19","2020-12-02","2020-12-10"};
    //dataRead("RE+4_R3_CH07", nPoints, dats, 6000);
    
    // Date good for 01, 02, 04   
    //std::string dats[nPoints] = {"2020-07-01","2020-10-19","2020-11-16","2020-12-10"};
    //dataRead("RE+4_R3_CH01", nPoints, dats, 6000); 
    //dataRead("RE+4_R3_CH02", nPoints, dats, 6000);
    //dataRead("RE+4_R3_CH04", nPoints, dats, 6000);
    
    // Date for 03
    //std::string dats[nPoints] = {"2020-07-01","2020-10-06","2020-11-16","2020-12-02"};
    //dataRead("RE+4_R3_CH03", nPoints, dats, 6000);

    // Date for 05
    //std::string dats[nPoints] = {"2020-10-06","2020-10-19","2020-10-31","2020-12-10"};
    //dataRead("RE+4_R3_CH05", nPoints, dats, 6000);

    // Date for 06
    //std::string dats[nPoints] = {"2020-07-01","2020-07-15","2020-10-19", "2020-12-10"};
    //dataRead("RE+4_R3_CH06", nPoints, dats, 6000);

    // 08: No data

    // Date for 09
    //std::string dats[nPoints] = {"2020-10-06","2020-10-19","2020-10-23","2020-12-10"};
    //dataRead("RE+4_R3_CH09", nPoints, dats, 6000);

    // Date for 10
    //std::string dats[nPoints] = {"2020-07-01","2020-07-15","2020-10-23","2020-12-10"};
    //dataRead("RE+4_R3_CH10", nPoints, dats, 6000);

    // Date for 11, 12, 13, 14, 16, 18, 21, 22, 29, 31, 32, 33, 34, 35
    //std::string dats[nPoints] = {"2020-07-01","2020-07-15","2020-10-23","2020-12-10"};
    //dataRead("RE+4_R2_CH35", nPoints, dats, 6000);

    // Date for 15, 17
    //std::string dats[nPoints] = {"2020-07-15","2020-10-19","2020-10-23", "2020-12-10"};
    //dataRead("RE+4_R2_CH17", nPoints, dats, 6000);

    // Date for 19
    //std::string dats[nPoints] = {"2020-10-06","2020-10-19","2020-10-23", "2020-12-10"};
    //dataRead("RE+4_R2_CH19", nPoints, dats, 6000);

    // Date for 20
    //std::string dats[nPoints] = {"2020-10-06","2020-11-16","2020-12-02", "2020-12-10"};
    //dataRead("RE+4_R2_CH20", nPoints, dats, 6000);

    // Date for 23
    //std::string dats[nPoints] = {"2020-07-01","2020-07-15","2020-10-19", "2020-12-10"};
    //dataRead("RE+4_R2_CH23", nPoints, dats, 6000);

    // Date for 25
    //std::string dats[nPoints] = {"2020-10-06","2020-10-19","2020-11-16", "2020-12-10"};
    //dataRead("RE+4_R2_CH25", nPoints, dats, 6000);

    // Date for 28
    //std::string dats[nPoints] = {"2020-08-28","2020-10-06","2020-10-19","2020-10-23"};
    //dataRead("RE+4_R2_CH28", nPoints, dats, 6000);

    // Date for 26
    //std::string dats[nPoints] = {"2020-10-06","2020-10-19","2020-10-23","2020-12-10"};
    //dataRead("RE+4_R2_CH26", nPoints, dats, 6000);

    // Date for 27
    //std::string dats[nPoints] = {"2020-07-01","2020-10-06","2020-10-19","2020-10-23"};
    //dataRead("RE+4_R2_CH27", nPoints, dats, 6000);

    // Date for 30
    //std::string dats[nPoints] = {"2020-07-01","2020-10-06","2020-10-19","2020-10-23"};
    //dataRead("RE+4_R2_CH30", nPoints, dats, 6000);

    // Date for 36

    std::string dats[nPoints] = {"2020-07-01","2020-07-15","2020-10-06","2020-12-10"};
    dataRead(csvfile, "RE+4_R2_CH36", nPoints, dats, 6000);
    
   
    

       
    
    return 0;
}

void test(){
    std::ofstream fileshow;
    fileshow.open("test.csv");
    fileshow << "Chamber, date, \n";
    fileshow << "sadsa";
    fileshow.close();
}