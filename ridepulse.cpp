#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <cctype>

using namespace std;

enum RideType
{
    ECONOMY = 1,
    PREMIUM = 2,
    SHARED = 3
};

bool isBackCommand(const string &input)
{
    string lowered;
    lowered.reserve(input.size());
    for (char ch : input)
    {
        lowered.push_back(static_cast<char>(tolower(static_cast<unsigned char>(ch))));
    }
    return lowered == "back" || lowered == "b";
}

double getPositiveDouble(const string &prompt)
{
    while (true)
    {
        cout << prompt;
        double v;
        if (cin >> v && v > 0)
            return v;
        cout << "❌ Invalid input. Please enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int getRideChoice(bool allowBack, bool &goBack)
{
    while (true)
    {
        goBack = false;
        cout << "\nChoose ride type:\n"
             << "1. Economy   (RM 1.50/km, min RM 4)\n"
             << "2. Premium   (RM 2.50/km, min RM 7)\n"
             << "3. Ride Share(RM 1.00/km, min RM 3)\n";
        cout << (allowBack ? "Enter choice (1-3 or 'back'): " : "Enter choice (1-3): ");
        string input;
        cin >> input;
        if (allowBack && isBackCommand(input))
        {
            goBack = true;
            return 0;
        }
        stringstream ss(input);
        int c;
        char extra;
        if ((ss >> c) && !(ss >> extra) && c >= 1 && c <= 3)
        {
            return c;
        }
        cout << "❌ Invalid choice. Please enter 1, 2, or 3";
        if (allowBack)
            cout << " (or type 'back' to return)";
        cout << ".\n";
    }
}

bool getYesNo(const string &prompt, bool allowBack, bool &goBack)
{
    while (true)
    {
        goBack = false;
        cout << prompt;
        string s;
        cin >> s;
        if (allowBack && isBackCommand(s))
        {
            goBack = true;
            return false;
        }
        if (s == "y" || s == "Y")
            return true;
        if (s == "n" || s == "N")
            return false;
        cout << "❌ Please enter y/n";
        if (allowBack)
            cout << " or 'back'";
        cout << ".\n";
    }
}

double round2(double v)
{
    return ((long long)(v * 100 + 0.5)) / 100.0;
}

int main()
{
    cout << "===============================\n";
    cout << "   Welcome to RidePulse App!   \n";
    cout << "===============================\n\n";
    cout << fixed << setprecision(2);

    const double RATE_ECON = 1.50;
    const double RATE_PREM = 2.50;
    const double RATE_SHARE = 1.00;
    const double MIN_ECON = 4.00;
    const double MIN_PREM = 7.00;
    const double MIN_SHARE = 3.00;
    const double BOOKING_FEE = 1.20; // flat booking/admin fee
    const double SERVICE_FEE = 0.05; // 5%
    const double SURGE_PEAK = 1.20;  // 20% increase at peak
    const string PROMO = "PULSE10";
    const double PROMO_DISC = 0.10; // 10%

    bool keepRunning = true;
    while (keepRunning)
    {
        double distance = 0.0;
        RideType type = ECONOMY;
        bool peak = false;
        string promo = "-";
        int step = 0;

        while (step < 4)
        {
            if (step == 0)
            {
                distance = getPositiveDouble("Enter your trip distance (km): ");
                step = 1;
            }
            else if (step == 1)
            {
                bool goBack = false;
                int choice = getRideChoice(true, goBack);
                if (goBack)
                {
                    cout << "↩️  Returning to distance entry.\n\n";
                    step = 0;
                    continue;
                }
                type = static_cast<RideType>(choice);
                step = 2;
            }
            else if (step == 2)
            {
                bool goBack = false;
                peak = getYesNo("Is it peak hours? (y/n or 'back' to change ride type): ", true, goBack);
                if (goBack)
                {
                    cout << "↩️  Returning to ride type selection.\n\n";
                    step = 1;
                    continue;
                }
                step = 3;
            }
            else if (step == 3)
            {
                cout << "Enter promo code (or - to skip, 'back' to review peak choice): ";
                cin >> promo;
                if (isBackCommand(promo))
                {
                    cout << "↩️  Returning to peak hours question.\n\n";
                    step = 2;
                    continue;
                }
                step = 4;
            }
        }

        // base calculation
        double perKm = (type == ECONOMY ? RATE_ECON : (type == PREMIUM ? RATE_PREM : RATE_SHARE));
        double minFare = (type == ECONOMY ? MIN_ECON : (type == PREMIUM ? MIN_PREM : MIN_SHARE));
        double distanceFare = distance * perKm;
        double baseFare = distanceFare < minFare ? minFare : distanceFare;

        // add booking fee
        double subtotal = baseFare + BOOKING_FEE;

        // apply surge if peak
        if (peak)
            subtotal *= SURGE_PEAK;

        // service fee
        double serviceFee = subtotal * SERVICE_FEE;
        subtotal += serviceFee;

        // promo
        double discount = 0.0;
        if (promo == PROMO)
        {
            discount = subtotal * PROMO_DISC;
            subtotal -= discount;
        }

        double total = round2(subtotal);

        // output summary
        cout << "\n========== Ride Summary ==========\n";
        cout << "Distance:           " << distance << " km\n";
        if (type == ECONOMY)
            cout << "Ride Type:          Economy\n";
        else if (type == PREMIUM)
            cout << "Ride Type:          Premium\n";
        else
            cout << "Ride Type:          Ride Share\n";
        cout << "Base (after min):   RM " << baseFare << "\n";
        cout << "Booking Fee:        RM " << BOOKING_FEE << "\n";
        if (peak)
            cout << "Peak Surge (x1.20): applied\n";
        else
            cout << "Peak Surge:         none\n";
        cout << "Service Fee (5%):   RM " << serviceFee << "\n";
        if (discount > 0)
            cout << "Promo (" << PROMO << "):   -RM " << discount << "\n";
        else
            cout << "Promo:              none\n";
        cout << "----------------------------------\n";
        cout << "Total Fare:         RM " << total << "\n";
        cout << "==================================\n";

        bool ignoreBack = false;
        keepRunning = getYesNo("\nAnother ride? (y/n): ", false, ignoreBack);
        cout << "\n";
    }

    cout << "Thank you for using RidePulse! \n";
    return 0;
}