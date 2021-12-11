#include <gmock/gmock.h>
#include <sstream>
#include "libabyss/formats/d2/mpq.h"
#include "libabyss/streams/mpqstream.h"
#include <stdexcept>

#define STORMLIB_NO_AUTO_LINK 1
#include <StormLib.h>

using testing::Eq;
using testing::ThrowsMessage;

class MpqTest : public testing::Test {
  public:
    MpqTest() {
        _fname = std::tmpnam(nullptr);
        _fname += ".mpq";
    }

    ~MpqTest() override { std::filesystem::remove(_fname); }

  protected:
    void WriteMPQ(std::function<void(HANDLE mpq)> data) {
        HANDLE mpq;
        ASSERT_TRUE(SFileCreateArchive(_fname.c_str(), MPQ_CREATE_LISTFILE, 100, &mpq)) << GetLastError();
        data(mpq);
        SFileCloseArchive(mpq);
    }
    static void WriteIn(HANDLE mpq, const std::string &fname, std::string_view data) {
        HANDLE f;
        ASSERT_TRUE(SFileCreateFile(mpq, fname.c_str(), 0, data.size(), 0, 0, &f)) << GetLastError();
        ASSERT_TRUE(SFileWriteFile(f, data.data(), data.length(), 0)) << GetLastError();
        ASSERT_TRUE(SFileFinishFile(f)) << GetLastError();
    }

    std::string _fname;
};

TEST_F(MpqTest, NonExistingMPQ) {
    EXPECT_THAT([&] { LibAbyss::MPQ mpq(_fname); }, ThrowsMessage<std::runtime_error>(Eq("Error occurred while opening MPQ " + _fname)));
}

TEST_F(MpqTest, NonExistingFile) {
    WriteMPQ([](HANDLE mpq) {});
    LibAbyss::MPQ mpq(_fname);
    EXPECT_THAT([&] { mpq.Load("non-existing.txt"); }, ThrowsMessage<std::runtime_error>(Eq("Failed to open file 'non-existing.txt' from MPQ")));
    EXPECT_FALSE(mpq.HasFile("non-existing.txt"));
}

TEST_F(MpqTest, SmallFile) {
    WriteMPQ([](HANDLE mpq) { WriteIn(mpq, "small.txt", "abcd"); });

    LibAbyss::MPQ mpq(_fname);

    LibAbyss::InputStream stream = mpq.Load("small.txt");
    EXPECT_EQ(stream.tellg(), 0);
    EXPECT_EQ(stream.get(), 'a');
    EXPECT_EQ(stream.tellg(), 1);
    EXPECT_EQ(stream.get(), 'b');
    stream.seekg(-1, std::ios_base::cur);
    EXPECT_EQ(stream.get(), 'b');
    stream.seekg(3);
    EXPECT_EQ(stream.get(), 'd');
    EXPECT_FALSE(stream.eof());
    EXPECT_EQ(stream.get(), -1);
    EXPECT_TRUE(stream.eof());
    stream.clear();
    stream.seekg(-2, std::ios_base::cur);
    EXPECT_EQ(stream.get(), 'c');
    stream.seekg(-2, std::ios_base::end);
    EXPECT_EQ(stream.get(), 'c');
    EXPECT_EQ(stream.tellg(), 3);
}

TEST_F(MpqTest, BigFile) {
    std::stringstream s;
    for (int i = 0; i < 10000; ++i) {
        s << i << ' ';
    }
    std::string source_str = s.str();
    std::string_view source = source_str;
    EXPECT_EQ(source.length(), 48890);
    EXPECT_EQ(source.substr(40000, 20), "8222 8223 8224 8225 ");
    // Near end of the chunk (+2048)
    EXPECT_EQ(source.substr(42040, 20), "8630 8631 8632 8633 ");

    WriteMPQ([&](HANDLE mpq) { WriteIn(mpq, "big.txt", source); });

    LibAbyss::MPQ mpq(_fname);
    LibAbyss::InputStream stream = mpq.Load("big.txt");
    LibAbyss::MPQStream* strbuf = dynamic_cast<LibAbyss::MPQStream*>(stream.rdbuf());
    ASSERT_NE(strbuf, nullptr);
    stream.seekg(40000);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 40000);
    int x;
    stream >> x;
    stream >> x;
    stream >> x;
    EXPECT_EQ(x, 8224);
    stream.seekg(42040);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 40000);
    stream.seekg(-10, std::ios_base::cur);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 40000);
    stream >> x;
    EXPECT_EQ(x, 8628);
    stream >> x;
    stream >> x;
    stream >> x;
    EXPECT_EQ(x, 8631);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 42048);
    stream.seekg(-10, std::ios_base::cur);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 42039);
    stream >> x;
    EXPECT_EQ(x, 8630);

    // Finally, read the whole file
    stream.seekg(0);
    for (int i = 0; i < 10000; ++i) {
        stream >> x;
        EXPECT_FALSE(stream.eof());
        EXPECT_EQ(i, x);
    }
    stream >> x;
    EXPECT_TRUE(stream.eof());
}

TEST_F(MpqTest, Slash) {
    WriteMPQ([](HANDLE mpq){ WriteIn(mpq, "a\\b", "xyz"); });
    LibAbyss::MPQ mpq(_fname);
    mpq.Load("a/b");
    mpq.Load("/a/b");
    mpq.Load("a\\b");
    EXPECT_TRUE(mpq.HasFile("a/b"));
    EXPECT_TRUE(mpq.HasFile("/a/b"));
    EXPECT_TRUE(mpq.HasFile("a\\b"));
}
