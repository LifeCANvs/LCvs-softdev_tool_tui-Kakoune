#include "unit_tests.hh"

#include "assert.hh"
#include "diff.hh"
#include "utf8.hh"
#include "string.hh"

namespace Kakoune
{

UnitTest test_utf8{[]()
{
    StringView str = "maïs mélange bientôt";
    kak_assert(utf8::distance(std::begin(str), std::end(str)) == 20);
    kak_assert(utf8::codepoint(std::begin(str) + 2, std::end(str)) == 0x00EF);
}};

UnitTest test_diff{[]()
{
    struct Diff{DiffOp op; int len; int posB;};
    auto check_diff = [](StringView a, StringView b, std::initializer_list<Diff> diffs) {
        size_t count = 0;
        for_each_diff(a.begin(), (int)a.length(), b.begin(), (int)b.length(),
                      [&](DiffOp op, int len, int posB) {
                          kak_assert(count < diffs.size());
                          auto& d = diffs.begin()[count++];
                          kak_assert(d.op == op and d.len == len and d.posB == posB);
                      });
        kak_assert(count == diffs.size());
    };
    check_diff("a?", "!", {{DiffOp::Remove, 1, 0}, {DiffOp::Add, 1, 0}, {DiffOp::Remove, 1, 0}});
    check_diff("abcde", "cd", {{DiffOp::Remove, 2, 0}, {DiffOp::Keep, 2, 0}, {DiffOp::Remove, 1, 0}});
    check_diff("abcd", "cdef", {{DiffOp::Remove, 2, 0}, {DiffOp::Keep, 2, 0}, {DiffOp::Add, 2, 2}});

    check_diff("mais que fais la police", "mais ou va la police",
               {{DiffOp::Keep, 5, 0}, {DiffOp::Remove, 1, 0}, {DiffOp::Add, 1, 5}, {DiffOp::Keep, 1, 0},
                {DiffOp::Remove, 1, 0}, {DiffOp::Keep, 1, 0}, {DiffOp::Add, 1, 8}, {DiffOp::Remove, 1, 0}, 
                {DiffOp::Keep, 1, 0}, {DiffOp::Remove, 2, 0}, {DiffOp::Keep, 10, 0}} );
}};

UnitTest* UnitTest::list = nullptr;

void UnitTest::run_all_tests()
{
    for (const UnitTest* test = UnitTest::list; test; test = test->next)
        test->func();
}

}
