/*
 * demo-drawtree.c
 *
 *  Created on: 01 Apr 2016
 *
 *  	This demonstration uses an array of data points to draw a tree on an oscilloscope.
 *      The oscilloscope will need to be set into x-y mode with probes on channels 1 and 2 to display the picture.
 *
 *      compile with "gcc ABE_ADCDACPi.c demo-drawtree.c -o demo-drawtree"
 *      run with "./demo-drawtree"
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ABE_ADCDACPi.h"

/* reddit alien */
//unsigned int xpoints[263] = {1304,1295,1260,1221,1143,1069,1012,969,930,912,908,930,960,1008,1077,1151,1217,1264,1304,1356,1482,1586,1677,1769,1825,1851,1873,1890,1912,1930,1947,1960,1986,2108,2203,2334,2421,2503,2577,2686,2747,2812,2882,2969,3051,3147,3203,3243,3260,3269,3247,3190,3147,3108,3090,3056,3030,2990,2951,2899,2847,2812,1964,1843,1751,1621,1512,1417,1351,1260,1195,1143,1069,986,869,804,756,743,734,751,799,860,904,912,925,969,1017,1056,1104,1138,873,886,934,977,1030,1086,1151,1225,1290,1377,1477,1573,1669,1747,1834,1925,2034,2134,2264,2421,2521,2582,2686,2777,2830,2899,2973,3051,3090,3112,3143,3143,2530,2530,2547,2560,2569,2564,2547,2543,2525,2512,2495,2486,2608,2669,2730,2782,2821,2830,2817,2803,2751,2717,2656,2608,2551,2460,2390,2364,2464,2569,2660,2712,2756,2769,2756,2673,2551,2460,2373,2282,2190,2251,2308,2325,2360,2134,2030,1912,1843,1717,1595,1499,1395,1295,1238,1217,1247,1282,1364,1447,1504,1556,1604,1647,1699,1760,1821,1599,1569,1547,1521,1508,1499,1477,1477,1447,1447,1451,1447,1451,1451,1443,1456,1464,1390,1330,1277,1225,1204,1169,1177,1177,1225,1238,1308,1386,1460,1504,1482,1612,1651,1699,1751,1799,1899,2008,2108,2212,2312,2356,2390,2412,2499,2547,2556,2547,2512,2464,2425,2364,2299,2264,2243,2277,2338,2399,2421,1738,1743,1708,1660,1599,1534,1490,1473,1473,1534,1595,1664,1717,1595};
//unsigned int ypoints[263] = {3617,3561,3496,3430,3404,3400,3417,3443,3491,3557,3626,3691,3735,3778,3783,3787,3761,3717,3661,3630,3657,3696,3709,3730,3735,3652,3578,3504,3426,3357,3283,3217,3161,3152,3143,3126,3109,3074,3043,3013,2965,2926,2952,2996,2991,2974,2935,2869,2804,2726,2643,2583,2535,2517,2587,2639,2691,2769,2813,2865,2891,2913,3148,3143,3126,3091,3052,3017,2996,2952,2913,2935,2974,2987,2974,2930,2856,2796,2709,2630,2578,2522,2504,2613,2665,2722,2774,2809,2843,2869,2435,2348,2252,2165,2074,2009,1943,1900,1843,1804,1765,1748,1722,1704,1687,1682,1678,1682,1717,1743,1769,1808,1843,1882,1930,1991,2052,2135,2213,2313,2400,2469,1730,1652,1535,1452,1365,1313,1217,1113,1021,926,791,734,804,852,921,1000,1139,1274,1352,1417,1508,1565,1626,1669,1700,687,591,517,513,508,469,421,365,313,239,234,221,221,217,217,230,291,360,404,465,221,221,217,230,217,217,217,217,217,226,282,374,452,504,526,530,526,508,439,387,313,247,539,621,682,769,856,930,1008,1100,1204,1269,1330,1400,1456,1526,1591,1652,1735,1661,1613,1548,1482,1408,1308,1226,1113,1030,930,869,804,761,752,1735,2091,2056,2026,2000,1978,1969,1965,1978,1995,2022,2048,2087,2417,2452,2513,2583,2630,2669,2691,2704,2704,2674,2609,2552,2500,2439,2469,2578,2530,2591,2643,2683,2700,2669,2630,2574,2496,2435,2422,2430,2469,2548};

/* tree */
unsigned int xpoints[231] = {1206,1230,1244,1288,1322,1374,1430,1498,1578,1638,1722,1776,1856,1930,2002,2084,2160,2232,2304,2382,2482,2538,2598,2656,2692,2732,2762,2776,2762,2726,2692,2650,2602,2562,2518,2464,2414,2366,2314,2270,2228,2196,2202,2200,2198,2194,2194,2194,2140,2070,2002,1948,1868,1810,1772,1758,1754,1756,1760,1756,1756,1713,1668,1621,1582,1532,1498,1431,1378,1324,1294,1234,1217,1198,1256,1756,1758,1550,1454,1368,1312,1202,1152,1066,1008,906,842,778,714,694,650,630,622,714,782,854,914,962,1036,1098,1136,1202,1112,1074,1032,996,958,918,902,964,1016,1064,1112,1144,1184,1220,1280,1328,1366,1418,1460,1492,1396,1330,1284,1246,1212,1186,1170,1240,1302,1336,1376,1428,1484,1528,1578,1630,1684,1706,1738,1776,1804,1832,1906,1938,1958,1986,2004,2016,2034,2052,2074,2108,2132,2176,2202,2238,2264,2302,2334,2376,2416,2466,2514,2570,2618,2670,2724,2768,2802,2840,2876,2802,2744,2696,2652,2596,2656,2698,2748,2794,2842,2890,2944,2992,3034,3072,3114,3152,3184,3112,3060,3012,2958,2922,3004,3054,3092,3138,3184,3242,3294,3350,3410,3446,3402,3370,3322,3272,3232,3168,3084,3024,2966,2914,2852,2776,2712,2628,2566,2480,2402,2330,2254,2196,2124,2060,1966,1892,1818};
unsigned int ypoints[231] = {652,618,588,554,530,492,470,446,432,416,408,396,394,394,400,402,404,412,420,434,460,476,506,534,560,600,652,700,748,814,846,886,914,930,954,970,986,996,1006,1012,1018,1022,970,916,874,810,744,694,652,636,628,632,638,660,686,712,786,858,936,994,1028,1017,1006,998,992,978,968,939,916,881,862,796,752,700,838,1062,1110,1138,1152,1172,1196,1220,1242,1276,1298,1360,1394,1440,1514,1556,1614,1660,1694,1736,1764,1800,1832,1862,1902,1938,1970,2006,2042,2066,2090,2122,2158,2204,2244,2262,2286,2316,2346,2366,2390,2412,2462,2492,2528,2566,2596,2630,2676,2698,2728,2756,2784,2816,2830,2858,2890,2908,2944,2990,3026,3070,3110,3176,3228,3264,3320,3354,3386,3426,3544,3578,3622,3672,3714,3750,3798,3794,3722,3678,3636,3574,3524,3472,3430,3388,3356,3270,3218,3178,3132,3086,3044,2992,2950,2916,2890,2860,2840,2766,2732,2706,2686,2658,2610,2582,2548,2510,2468,2426,2382,2344,2324,2292,2270,2246,2230,2146,2098,2072,2050,2032,1980,1944,1918,1888,1858,1816,1784,1756,1718,1688,1602,1536,1486,1440,1410,1370,1318,1282,1258,1240,1222,1196,1182,1164,1146,1138,1122,1108,1098,1098,1094,1094,1086,1090,1100};


int i;
int arraysize;

int main(int argc, char **argv){
	setvbuf (stdout, NULL, _IONBF, 0); // needed to print to the command line

	if (open_dac() != 1){ // open the DAC spi channel
		exit(1); // if the SPI bus fails to open exit the program
	}

	set_dac_gain(1); // set the dac gain to 1 which will give a voltage range of 0 to 2.048V

	arraysize = sizeof(xpoints) / sizeof(unsigned int);;

	while (1){
				for (i=0; i<arraysize; ++i){
					set_dac_raw(xpoints[i],2);
					set_dac_raw(ypoints[i],1);
				}
	}

	close_dac();
	return (0);
}
