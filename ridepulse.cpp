#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <cctype>
#include <stdexcept>
#include <cmath>
  
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

[[noreturn]] void throwInputTerminated(const string &context)
{
    throw runtime_error("Input stream closed while " + context + ".");
}

bool equalsIgnoreCase(const string &lhs, const string &rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    for (size_t i = 0; i < lhs.size(); ++i)
    {
        if (tolower(static_cast<unsigned char>(lhs[i])) != tolower(static_cast<unsigned char>(rhs[i])))
            return false;
    }
    return true;
}

string trim(const string &value)
{
    size_t start = 0;
    while (start < value.size() && isspace(static_cast<unsigned char>(value[start])))
    {
        ++start;
    }
    size_t end = value.size();
    while (end > start && isspace(static_cast<unsigned char>(value[end - 1])))
    {
        --end;
    }
    return value.substr(start, end - start);
}

bool promptAndReadLine(const string &prompt, string &line, const string &context)
{
    cout << prompt;
    if (!getline(cin, line))
    {
        if (cin.eof())
            throwInputTerminated(context);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

double getPositiveDouble(const string &prompt)
{
    while (true)
    {
        string line;
        if (!promptAndReadLine(prompt, line, "entering the trip distance"))
        {
            cout << "❌ Invalid input. Please enter a positive number.\n";
            continue;
        }
        string trimmed = trim(line);
        if (trimmed.empty())
        {
            cout << "❌ Invalid input. Please enter a positive number.\n";
            continue;
        }
        try
        {
            size_t idx = 0;
            double value = stod(trimmed, &idx);
            if (idx == trimmed.size() && value > 0.0)
                return value;
        }
        catch (const invalid_argument &)
        {
        }
        catch (const out_of_range &)
        {
        }
        cout << "❌ Invalid input. Please enter a positive number.\n";
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
        string input;
        if (!promptAndReadLine(allowBack ? "Enter choice (1-3 or 'back'): " : "Enter choice (1-3): ", input, "selecting a ride type"))
        {

            cout << "❌ Invalid choice. Please enter 1, 2, or 3";
            if (allowBack)
                cout << " (or type 'back' to return)";
            cout << ".\n";
            continue;
        }
        string trimmed = trim(input);
        if (trimmed.empty())
        {
            cout << "❌ Invalid choice. Please enter 1, 2, or 3";
            if (allowBack)
                cout << " (or type 'back' to return)";
            cout << ".\n";
            continue;
        }
        if (allowBack && isBackCommand(trimmed))
        {
            goBack = true;
            return 0;
        }
        try
        {
            size_t idx = 0;
            int choice = stoi(trimmed, &idx);
            if (idx == trimmed.size() && choice >= 1 && choice <= 3)
            {
                return choice;
            }
        }
        catch (const invalid_argument &)
        {
        }
        catch (const out_of_range &)
        {
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

        string s;
        if (!promptAndReadLine(prompt, s, "answering a yes/no question"))
        {
            cout << "❌ Please enter y/n";
            if (allowBack)
                cout << " or 'back'";
            cout << ".\n";
            continue;
        }
        string trimmed = trim(s);
        if (trimmed.empty())
        {
            cout << "❌ Please enter y/n";
            if (allowBack)
                cout << " or 'back'";
            cout << ".\n";
            continue;
        }
        if (allowBack && isBackCommand(trimmed))
        {
            goBack = true;
            return false;
        }
        if (equalsIgnoreCase(trimmed, "y") || equalsIgnoreCase(trimmed, "yes"))
            return true;
        if (equalsIgnoreCase(trimmed, "n") || equalsIgnoreCase(trimmed, "no"))
            return false;
        cout << "❌ Please enter y/n";
        if (allowBack)
            cout << " or 'back'";
        cout << ".\n";
    }
}

double round2(double v)
{
    return std::round(v * 100.0 + std::copysign(1e-9, v)) / 100.0;
}

int main()
{
    try
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
            string promoInput = "-";
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
                    string promoLine;
                    if (!promptAndReadLine("Enter promo code (or - to skip, 'back' to review peak choice): ", promoLine, "entering a promo code"))
                    {
                        cout << "❌ Invalid input. Please try again.\n";
                        continue;
                    }
                    string trimmedPromo = trim(promoLine);
                    if (trimmedPromo.empty())
                    {
                        cout << "❌ Invalid input. Please try again.\n";
                        continue;
                    }
                    if (isBackCommand(trimmedPromo))
                    {
                        cout << "↩️  Returning to peak hours question.\n\n";
                        step = 2;
                        continue;
                    }
                    promoInput = trimmedPromo;
                    if (promoInput != "-" && !equalsIgnoreCase(promoInput, PROMO))
                    {
                        cout << "⚠️ Promo code not recognized. No discount will be applied.\n";
                    }
                    step = 4;
                }
            }
            // base calculation
            double perKm = (type == ECONOMY ? RATE_ECON : (type == PREMIUM ? RATE_PREM : RATE_SHARE));
            double minFare = (type == ECONOMY ? MIN_ECON : (type == PREMIUM ? MIN_PREM : MIN_SHARE));
            double distanceFare = distance * perKm;
            double baseFare = distanceFare < minFare ? minFare : distanceFare;

            double surgedFare = peak ? baseFare * SURGE_PEAK : baseFare;
            double subtotalBeforeService = surgedFare + BOOKING_FEE;

            double serviceFee = subtotalBeforeService * SERVICE_FEE;
            double subtotal = subtotalBeforeService + serviceFee;

            // promo
            double discount = 0.0;
            bool promoApplied = false;
            bool promoSkipped = (promoInput == "-");
            if (!promoSkipped)
            {
                if (equalsIgnoreCase(promoInput, PROMO))
                {
                    discount = subtotal * PROMO_DISC;
                    subtotal -= discount;
                    promoApplied = true;
                }
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
            if (promoApplied)
                cout << "Promo (" << PROMO << "):   -RM " << discount << "\n";
            else if (!promoSkipped)
                cout << "Promo:              invalid (" << promoInput << ")\n";
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

    catch (const runtime_error &ex)
    {
        cerr << "\nFatal error: " << ex.what() << "\n";
    }
    return 1;
}
