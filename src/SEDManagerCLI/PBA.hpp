#pragma once


class PBA {
public:
    PBA();
    PBA(const PBA&) = delete;
    PBA(PBA&&) = delete;
    PBA& operator=(const PBA&) = delete;
    PBA& operator=(PBA&&) = delete;

    int Run();

private:
    bool m_finished = false;
};