#pragma once

#include <iostream>
#include <mutex>
#include <iomanip>

class ProgressBar {
public:
	std::mutex mtx;
	int now;
	int range;
	std::string info;

	ProgressBar(std::string i, int r) : now(0), range(r), info(i) {};

	/**
	 * @brief 进度显示
	 * @param info 进度信息显示
	 * @param now 任务完成数量（需传入引用）
	 * @param range 最大数量
	 * @param mtx 互斥锁引用
	*/
	void updateProgressBar() {
		if (range < 1) return;

		mtx.lock();
		now++;
		std::cout << info << "... " << now << "/" << range << " " << std::fixed << std::setprecision(2) << (float)now / (float)range * 100 << "%";
		if (now >= range) std::cout << std::endl;
		else std::cout << "\r";
		mtx.unlock();
	}
};