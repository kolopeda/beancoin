#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BEA");
    BOOST_CHECK(GetBoolArg("-BEA"));
    BOOST_CHECK(GetBoolArg("-BEA", false));
    BOOST_CHECK(GetBoolArg("-BEA", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BEAo"));
    BOOST_CHECK(!GetBoolArg("-BEAo", false));
    BOOST_CHECK(GetBoolArg("-BEAo", true));

    ResetArgs("-BEA=0");
    BOOST_CHECK(!GetBoolArg("-BEA"));
    BOOST_CHECK(!GetBoolArg("-BEA", false));
    BOOST_CHECK(!GetBoolArg("-BEA", true));

    ResetArgs("-BEA=1");
    BOOST_CHECK(GetBoolArg("-BEA"));
    BOOST_CHECK(GetBoolArg("-BEA", false));
    BOOST_CHECK(GetBoolArg("-BEA", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBEA");
    BOOST_CHECK(!GetBoolArg("-BEA"));
    BOOST_CHECK(!GetBoolArg("-BEA", false));
    BOOST_CHECK(!GetBoolArg("-BEA", true));

    ResetArgs("-noBEA=1");
    BOOST_CHECK(!GetBoolArg("-BEA"));
    BOOST_CHECK(!GetBoolArg("-BEA", false));
    BOOST_CHECK(!GetBoolArg("-BEA", true));

    ResetArgs("-BEA -noBEA");  // -BEA should win
    BOOST_CHECK(GetBoolArg("-BEA"));
    BOOST_CHECK(GetBoolArg("-BEA", false));
    BOOST_CHECK(GetBoolArg("-BEA", true));

    ResetArgs("-BEA=1 -noBEA=1");  // -BEA should win
    BOOST_CHECK(GetBoolArg("-BEA"));
    BOOST_CHECK(GetBoolArg("-BEA", false));
    BOOST_CHECK(GetBoolArg("-BEA", true));

    ResetArgs("-BEA=0 -noBEA=0");  // -BEA should win
    BOOST_CHECK(!GetBoolArg("-BEA"));
    BOOST_CHECK(!GetBoolArg("-BEA", false));
    BOOST_CHECK(!GetBoolArg("-BEA", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BEA=1");
    BOOST_CHECK(GetBoolArg("-BEA"));
    BOOST_CHECK(GetBoolArg("-BEA", false));
    BOOST_CHECK(GetBoolArg("-BEA", true));

    ResetArgs("--noBEA=1");
    BOOST_CHECK(!GetBoolArg("-BEA"));
    BOOST_CHECK(!GetBoolArg("-BEA", false));
    BOOST_CHECK(!GetBoolArg("-BEA", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BEA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BEA", "eleven"), "eleven");

    ResetArgs("-BEA -bar");
    BOOST_CHECK_EQUAL(GetArg("-BEA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BEA", "eleven"), "");

    ResetArgs("-BEA=");
    BOOST_CHECK_EQUAL(GetArg("-BEA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BEA", "eleven"), "");

    ResetArgs("-BEA=11");
    BOOST_CHECK_EQUAL(GetArg("-BEA", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BEA", "eleven"), "11");

    ResetArgs("-BEA=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BEA", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BEA", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BEA", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BEA", 0), 0);

    ResetArgs("-BEA -bar");
    BOOST_CHECK_EQUAL(GetArg("-BEA", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BEA=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BEA", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BEA=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BEA", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BEA");
    BOOST_CHECK_EQUAL(GetBoolArg("-BEA"), true);

    ResetArgs("--BEA=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BEA", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBEA");
    BOOST_CHECK(!GetBoolArg("-BEA"));
    BOOST_CHECK(!GetBoolArg("-BEA", true));
    BOOST_CHECK(!GetBoolArg("-BEA", false));

    ResetArgs("-noBEA=1");
    BOOST_CHECK(!GetBoolArg("-BEA"));
    BOOST_CHECK(!GetBoolArg("-BEA", true));
    BOOST_CHECK(!GetBoolArg("-BEA", false));

    ResetArgs("-noBEA=0");
    BOOST_CHECK(GetBoolArg("-BEA"));
    BOOST_CHECK(GetBoolArg("-BEA", true));
    BOOST_CHECK(GetBoolArg("-BEA", false));

    ResetArgs("-BEA --noBEA");
    BOOST_CHECK(GetBoolArg("-BEA"));

    ResetArgs("-noBEA -BEA"); // BEA always wins:
    BOOST_CHECK(GetBoolArg("-BEA"));
}

BOOST_AUTO_TEST_SUITE_END()
