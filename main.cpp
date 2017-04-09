
/*
Author: Cordell Hurst
Course : CSCI 136 Supervised Programming Lab
Instructor: Gilbert Pajela
Assignment: Lab 8
09 April 2017
This program is tasked to read an input file, parse its lines, create a struct called 
'Country' for each line and places that struct into the array of a meta struct called 
'World'. As each line of data is read, its first 5 fields are real numbers and the first 
4 of those are used to create new numbers that are averages of every pair in the order 
of intake for a total of 9 numerical data elements. Those elements are placed in an 
unordered array called 'Pops' of a 'Country' struct. The second and last number are used 
to find a growth rate and the result is assigned to 'growth_rate'. The final data element 
from the line is taken as a string and assigned to 'name' element within the 'Country' 
struct. This struct is placed in an array called 'Countries' in the 'World' struct. A count
of the number of 'Country' elements placed is kept as 'num_countries' as well as the 
'Country' with the fastest growth rate (fastest_growth), the slowest growth rate (slowest_growth)
and the quantity of negative growth rate countries (num_slowest). The countries with negative 
growth rates are additionally placed into an array called 'NegativeRates' within the 'World' struct.
This array is sorted using insertion sort for a cost of O(N^2)
Country with fastest rate of growth is printed out as well as that with the slowest.
Finally, all negative growth countries are printed in order of 2015 population.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

const int MAXCOUNTRIES = 228;

struct Country 
{
	string name;
	double growth_rate;
	double Pops[8];  // stores population in 1950, 1960,..., 2010, 2015 in order
};
struct World 
{
	int num_countries, fastest_growth, slowest_growth, num_slowest;
	Country Countries[MAXCOUNTRIES];
	Country NegativeRates[8];
} myWorld;

Country ProcessFile(ifstream &PopFile, Country CountryIn);
void NegativeGrowthSort();
void PrintMagic();

int main()
{
	double temp = 0;
	double temp_fastest_growth = 0, temp_slowest_growth = 0;
	
	ifstream PopFile("population.csv");
	if (!PopFile.is_open())
	{
		cerr << "\n\tCould not open file\n\ns";
		return 1;
	}

	Country CountryIn;
	myWorld.num_countries = 0;
	int i = 0;
	while (temp != EOF)
	{
		myWorld.Countries[i] = ProcessFile(PopFile, CountryIn);
		//comparing elements to find index of country with fastest growth rate
		if (myWorld.Countries[i].growth_rate > temp_fastest_growth)
		{
			temp_fastest_growth = myWorld.Countries[i].growth_rate;
			myWorld.fastest_growth = i;
		}
		//comparing elements to find index of country with slowest growth rate
		if (myWorld.Countries[i].growth_rate < temp_slowest_growth)
		{
			temp_slowest_growth = myWorld.Countries[i].growth_rate;
			myWorld.slowest_growth = i;
		}
		myWorld.num_countries++;
		i++;
		temp = PopFile.peek();
	}

	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);
	cout.precision(2);
	cout << "\nThe fastest growing country is " << myWorld.Countries[myWorld.fastest_growth].name <<
			", which grew by " << myWorld.Countries[myWorld.fastest_growth].growth_rate << 
			"% between 1960 and 2015.\n";
	cout << "The slowest growing country is " << myWorld.Countries[myWorld.slowest_growth].name <<
			", which shrunk by " << myWorld.Countries[myWorld.slowest_growth].growth_rate*(-1) << //multiplied by -1 to make grammatical sense
			"% between 1960 and 2015.\n";
	
	NegativeGrowthSort();
	PrintMagic();

	return 0;
}
//Precondition: successfully opened datafile.
//Postcondition: line from data file is read and appropriate elements of 'Country' struct
//are populated accordingly. Growth rate is calculated using the  "straight-line" method
//and Country struct is returned.
Country ProcessFile(ifstream &PopFile, Country CountryIn)
{
	double year50, year60, year70, year80, year90, year00, year10, year15;

	PopFile >> year50;//first
	PopFile >> year70;//second
	year60 = (year50 + year70) / 2;

	PopFile >> year90;//third
	year80 = (year70 + year90) / 2;

	PopFile >> year10;//fourth
	year00 = (year90 + year10) / 2;

	PopFile >> year15;//fifth

	getline(PopFile, CountryIn.name);//country name

	//to remove leading space.
	if (CountryIn.name[0] = ' ')
	{
		string temp_str;
		int i = 1;
		while (i < CountryIn.name.size())
		{
			temp_str += CountryIn.name[i];
			i++;
		}
		CountryIn.name = temp_str;
	}
	//array assignment - so short, loop unnecessary
	CountryIn.Pops[0] = year50;
	CountryIn.Pops[1] = year60;
	CountryIn.Pops[2] = year70;
	CountryIn.Pops[3] = year70;
	CountryIn.Pops[4] = year80;
	CountryIn.Pops[5] = year90;
	CountryIn.Pops[6] = year00;
	CountryIn.Pops[7] = year10;
	CountryIn.Pops[8] = year15;
	//straight-line method - not really appropriate for comparisons but im guessing is expected here.
	CountryIn.growth_rate = ((year15 - year60)/year60) * 100; 
	
	return CountryIn;
}

//Precondition: Unsorted array of countries in myWorld struct, count of countries,
//count of negative growth countries.
//Postcondition: Sorted array of negative growth countrie called NegativeRates in
//myWorld struct, sorted by 2015 population in ascending order
void NegativeGrowthSort()
{
	myWorld.num_slowest = 0;
	int i = 0;
	int j = 1;

	Country blank;
	blank.name = "";
	blank.growth_rate = 0.0;
	for (int i = 0; i < 8; i++)//zero out array - need empty element for sort and I was getting garbage
	{
		myWorld.NegativeRates[i] = blank;
	}
	i = 0;
	while (i < myWorld.num_countries)//finding negative growth countries
	{
		if (myWorld.Countries[i].growth_rate < 0)
		{
			myWorld.NegativeRates[j] = myWorld.Countries[i];
			myWorld.num_slowest++; //count of negative growth countries
			j++;
		}
		i++;
	}
	Country temp;
	i = 0;
	//insertion sort @ O(N^2). Would prefer to use std::set::emplace for sort on insertion @ O(logN)
	for (i = 0; i < myWorld.num_slowest + 1; i++)
	{
		temp = myWorld.NegativeRates[i];
		j = i;
		while (j > 0 && myWorld.NegativeRates[j - 1].Pops[8] > temp.Pops[8])//sorted by 2015 population
		{
			myWorld.NegativeRates[j] = myWorld.NegativeRates[j - 1];
			j = j - 1;
		}
		myWorld.NegativeRates[j] = temp;
	}
}
//Precondition:: Array of negative growth countries called NegativeRates in myWorld struct, 
//sorted by 2015 population in ascending order
//Postcondition:: Information about countries within array displayed in neat columns
void PrintMagic()
{
	cout << "\nThe countries that shrank between 1960 and 2015 (and their 'growth' rates) are:\n\n";
	
	for (int i = 0; i < 7; i++)//one element is not to be printed. blank
	{	
		cout << fixed;
		cout << "\t" << setw(15) << left <<  myWorld.NegativeRates[i].name  << right  << setw(7)  << myWorld.NegativeRates[i].growth_rate << "%\n";
	}
	cout << "\n";
}

