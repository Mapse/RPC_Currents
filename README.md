# RPC Currents Histograms

For getting the RPCs currents histograms the following packages ared needed:

 * **jupyter notebook:** web-based interactive computing platform.
 * **pandas:** data manipulation.
 * **matplotlib:** plotting.
 * **numpy:** mathematica functions.
 * **mplhep:** plot in CMS style.
 
 The code used for the plots is **Plot_current_distributions.ipynb**. You have two input csv files, one containing the DPID number to chamber name conversion and another with the data needed to plot (dates, currents, voltage, chamber name).
 
 ## Running the code
 
 First you need to read the csv file in one pandas dataframe. The csv with data is the one obtained in DBeaver. It must have the following columns:
 
| DPID  | DATE  | START_DATE   | STOP_DATE | V_APPLIED  | AVG_IMON |

To read the data:
```  
data_path = "data_october_2021.csv"

df_data = pd.read_csv(data_path, parse_dates=True)
```
After that you need to get the mapping between DPID and chamber:

```
mapping_file_path = "RPC_Chamber_HV_Map.csv"
df_mapping = pd.read_csv(mapping_file_path, parse_dates=True)
```

In this point you need use either the function for selecting barrel chambers (individual_wheel) or endcap chambers (select_endcap).

For example, for wheel -1:

```
voltage = 8000
Wminus1 = individual_wheel('W-1', voltage, df_data = convert_dpid_name(df_data, df_mapping))
```

To plot you need to provide the data frame Wminus1, a numpy array with dates, and the number of bins of the histogram,


```
dates_w2 = np.array(['2021-03-03', '2021-04-13', '2021-07-22', '2021-10-14']) #'year-month-day'
plot(Wminus2, dates_w2, bins=30)
```

Is worth mentioning that all plots are already on the notebook.

 **IMPORTANT!!!**
 
 Not always the datas on the csv file will be unique for a set of barrel/endcap chambers. For example: RE4/2/4 finished the HV scan in 2021-03-03 and RE4/2/28 finished the HV scan in 2021-03-10. Therefore, either you need to put he same dates for both chambers or improve the method so that it can read multiples dates for a single set of detectors. 
