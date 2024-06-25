include(FetchContent)

Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    dawn
    GIT_REPOSITORY  https://dawn.googlesource.com/dawn
    GIT_TAG         chromium/6478
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(dawn)