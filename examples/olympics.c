#include <stc/csmap.h>
#include <stc/clist.h>
#include <stc/cstr.h>
#include <stdio.h>

// Olympics multimap example

struct OlympicsData { int year; const char *city, *country, *date; } ol_data[] = {
    {2026, "Milan and Cortina d'Ampezzo", "Italy", "February 6-22"},
    {2022, "Beijing", "China", "February 4-20"},
    {2018, "PyeongChang", "South Korea", "February 9-25"},
    {2014, "Sochi", "Russia", "February 7-23"},
    {2010, "Vancouver", "Canada", "February 12-28"},
    {2006, "Torino", "Italy", "February 10-26"},
    {2002, "Salt Lake City", "United States", "February 8-24"},
    {1998, "Nagano", "Japan", "February 7-22"},
    {1994, "Lillehammer", "Norway", "February 12-27"},
    {1992, "Albertville", "France", "February 8-23"},
    {1988, "Calgary", "Canada", "February 13-28"},
    {1984, "Sarajevo", "Yugoslavia", "February 8-19"},
    {1980, "Lake Placid", "United States", "February 13-24"},
    {1976, "Innsbruck", "Austria", "February 4-15"},
    {1972, "Sapporo", "Japan", "February 3-13"},
    {1968, "Grenoble", "France", "February 6-18"},
    {1964, "Innsbruck", "Austria", "January 29-February 9"},
    {1960, "Squaw Valley", "United States", "February 18-28"},
    {1956, "Cortina d'Ampezzo", "Italy", "January 26 - February 5"},
    {1952, "Oslo", "Norway", "February 14 - 25"},
    {1948, "St. Moritz", "Switzerland", "January 30 - February 8"},
    {1944, "canceled", "canceled", "canceled"},
    {1940, "canceled", "canceled", "canceled"},
    {1936, "Garmisch-Partenkirchen", "Germany", "February 6 - 16"},
    {1932, "Lake Placid", "United States", "February 4 - 15"},
    {1928, "St. Moritz", "Switzerland", "February 11 - 19"},
    {1924, "Chamonix", "France", "January 25 - February 5"},
};


typedef struct { int year; cstr city, date; } OlympicLocation;

int OlympicLocation_compare(OlympicLocation* a, OlympicLocation* b) {
    return a->year - b->year;
}
void OlympicLocation_del(OlympicLocation* self) {
    c_del (cstr, &self->city, &self->date);
}

// Create a clist<OlympicLocation>, can be sorted by year.
using_clist(ol, OlympicLocation, OlympicLocation_compare, OlympicLocation_del);

// Create a csmap<cstr, clist_ol> where key is country
using_csmap_strkey(ol, clist_ol, clist_ol_del, c_no_clone);


int main()
{
    c_withvar (csmap_ol, multimap) {
        clist_ol empty = clist_ol_init();

        for (int i = 0; i<c_arraylen(ol_data); ++i) {
            struct OlympicsData* it = &ol_data[i];
            OlympicLocation loc = {.year=it->year, .city=cstr_from(it->city), .date=cstr_from(it->date)};
            // Insert an empty list for each new country, and append the entry to the list.
            // If list already exist in map, it returns it from the insert function.
            clist_ol_push_back(&csmap_ol_insert(&multimap, cstr_from(it->country), empty).ref->second, loc);
        }

        c_foreach (country, csmap_ol, multimap) {
            clist_ol_sort(&country.ref->second); // Sort locations by year for each country.
            c_foreach (loc, clist_ol, country.ref->second)
                printf("%s: %d, %s, %s\n", country.ref->first.str, loc.ref->year,
                                                                   loc.ref->city.str,
                                                                   loc.ref->date.str);
        }
    }
}
