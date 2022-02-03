# CPP Tests Vulkan

## Known issues

1. In debug mode (`NDEBUG = false`) validation layers are used. This sometimes segfaults at least on MacOS/MoltenVK but eventually works.
    - ToDo: implement messaging so initialization can properly talk back and see if there are more insights
