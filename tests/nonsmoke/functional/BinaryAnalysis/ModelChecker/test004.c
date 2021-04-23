void reachable(void) {}
void unreachable(void) {}

// This test should find 32 calls to "reachable"
void test(int a, int b, int c, int d, int e) {
    if (a) {
        if (b) {
            if (c) {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            } else {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            }
        } else {
            if (c) {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            } else {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            }
        }
    } else {
        if (b) {
            if (c) {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            } else {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            }
        } else {
            if (c) {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            } else {
                if (d) {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                } else {
                    if (e) {
                        reachable();
                    } else {
                        reachable();
                    }
                }
            }
        }
    }
}

int main() {
    return 0;
}
