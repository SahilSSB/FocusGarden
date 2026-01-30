#pragma once
#include <cmath>
#include <algorithm>

class ExperienceSystem {
  public: 
      ExperienceSystem() : mLevel(1), mCurrentXP(0), mNextLevelThreshold(100) {
        calculateNextThreshold();
      }

      bool addXP(int amount) {
        mCurrentXP += amount;

        bool leveledUP = false;
        while (mCurrentXP >= mNextLevelThreshold) {
            mCurrentXP -= mNextLevelThreshold;
            mLevel++;
            leveledUP = true;
            calculateNextThreshold();
        }
        return leveledUP;
      }

      int getLevel() const { return mLevel; }
      int getCurrentXP() const { return mCurrentXP; }
      int getNextLevelThreshold() const { return mNextLevelThreshold; }

      float getProgress() const {
        if (mNextLevelThreshold == 0) return 0.f;
        return static_cast<float>(mCurrentXP) / static_cast<float>(mNextLevelThreshold);
      }
      
      void setLevel(int level) {
        mLevel = level;
        calculateNextThreshold();
      }

      void setCurrentXP(int xp) { mCurrentXP = xp; }

    private:
      int mLevel;
      int mCurrentXP;
      int mNextLevelThreshold;

      void calculateNextThreshold() {
        mNextLevelThreshold = static_cast<int>(100.f * std::pow(mLevel, 1.2f));
      }
};
