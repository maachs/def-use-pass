#include <gtest/gtest.h>
#include "Common.hpp"
#include <cstdlib>

TEST(PathTest, EnvironmentVariable) {

    setenv("DEFUSE_PATH", "custom_path.dot", 1);

    EXPECT_STREQ(get_defuse_path(), "custom_path.dot");
}

TEST(PathTest, DefaultValue) {

    unsetenv("DEFUSE_PATH");

    EXPECT_STRNE(get_defuse_path(), "");
}
