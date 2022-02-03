# CPP Tests Vulkan

## Known issues

1. In debug mode (`NDEBUG = false`) validation layers are used. This sometimes segfaults but eventually works.
    - ToDo: implement messaging so initialization can properly talk back and see if there are more insights
    - ToDo: ended up disabling validation layers until this is solved
