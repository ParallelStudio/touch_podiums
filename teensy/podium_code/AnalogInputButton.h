#ifndef __ANALOG_INPUT_BUTTON_H__
#define __ANALOG_INPUT_BUTTON_H__

struct AnalogInputButtonInit {
  int32_t threshold; //turn on @ baseline+threshold+hysteresis, off @baseline+threshold-hysteresis. Threshold can be negative
  uint32_t hysteresis; //see above, adjusts threshold for noise immunity
  uint32_t baseline_period_ticks; //how often to increment/decrement baseline in ticks
  bool baseline_always; //adjust baseline regardless of sensor state and noise level
  uint32_t timeout_ticks; //if nonzero, max sensor on time in ticks. Sensor state and baseline will reset if on that long.
  uint32_t noise; //noise level for baseline adjustent, unless baseline_always
};

class AnalogInputButton {
  public:
    AnalogInputButton(const AnalogInputButtonInit *const init)
    : mInitialized(false)
    , mSwitchState(false)
    , mChanged(false)
    , mLastResult(0)
    , mBaseline(0) 
    , mThreshold(init->threshold)
    , mHysteresis(init->hysteresis)
    , mNoise(init->noise)
    , mBaselineCount(0) 
    , mBaselineRate(init->baseline_period_ticks)
    , mBaselineAlways(init->baseline_always)
    , mTimeOn(0)
    , mTimeoutTicks(init->timeout_ticks) {
    }

    bool Update(const int32_t value) {
      mLastResult = value;

      //check baseline initialization flag
      //reset baseline value if flag is unset
      if (!mInitialized) {
        mBaseline=mLastResult;
        mInitialized=true;
        mBaselineCount=0;
      }

      //compute sensor level as difference between raw and baseline
      //if threshold negative, reverse sign and do everything else as though threshold positive
      const int32_t difference = mLastResult - mBaseline * (mThreshold > 0 ? 1 : -1);

      //adjust baseline
      //Baseline drifts toward result value when switch off or mBaselineAlways set
      //baseline not affected while switch on if mBaselineAlways is false
      //Baseline adjustment rate determined by mBaselineRate
      mBaselineCount = (mBaselineCount+1) % mBaselineRate;
      if (mBaselineCount==0 && (mBaselineAlways || abs(difference) < mNoise)) {

        //reset baseline if negative by more than threshold value
        if (difference < -mThreshold) {
          mBaseline=mLastResult;
        } else {
          mBaseline += difference > 0 ? 1 :
                       (difference < 0 ? -1 : 0);
        }
      }

      //Check switch state
      if (!mSwitchState && difference > (mThreshold+mHysteresis)) {
        mSwitchState = true;
        mChanged = true;
        mTimeOn = 0;
      } else if (mSwitchState && difference < (mThreshold-mHysteresis)) {
        mSwitchState=false;
        mChanged=true;
      } else {
      }

      //if on, check timeout
      if (mSwitchState && mTimeoutTicks!=0) {
        
        if (mTimeOn > mTimeoutTicks) {
          //flag to reset next time around
          mInitialized=false;
        }
      }

      //increment on time counter
      if (mSwitchState) {
        mTimeOn += 1;
      }

      return mSwitchState;
    }

    bool State() const {
      return mSwitchState;
    }

    bool Changed() {
      bool rVal=mChanged;
      mChanged=false;
      return rVal;
    }

    int32_t LastResult() const {
      return mLastResult;
    }

    int32_t Baseline() const {
      return mBaseline;
    }

    void ResetBaseline() {
      mInitialized=0;
    }

    int32_t Diff() const {
      return LastResult() - Baseline();
    }

    uint16_t BaselineRate() const {
      return mBaselineRate;
    }

    void SetBaselineRate (const uint16_t val) {
      mBaselineRate=val;
    }

    int32_t Hysteresis() const {
      return mHysteresis;
    }

    void SetHysteresis(const int32_t val) {
      if (val<0) {
        mHysteresis=0;
      } else {
        mHysteresis=val;
      }
    }

    uint32_t TimeoutTicks() const {
      return mTimeoutTicks;
    }

    void SetTimeoutTicks(const uint32_t val) {
      mTimeoutTicks=val;
    }

    bool BaselineAlways() const {
      return mBaselineAlways;
    }

    void SetBaselineAlways(const bool val) {
      mBaselineAlways=val;
    }

  private:
    bool mInitialized; //has the baseline been initialized? Also used for reset.
    bool mSwitchState; //Is the switch off or on?
    bool mChanged; //Did the switch state just change?

    int32_t mLastResult; //Last raw result value from sensor.
    int32_t mBaseline; //Sensor baseline value.
    int32_t mThreshold; //on/off threshold
    int32_t mHysteresis; //on/off hysteresis
    int32_t mNoise;

    int32_t mBaselineCount; //counter to control baseline adjustment rate
    int32_t mBaselineRate; //baseline adjustment counter period

    bool mBaselineAlways;

    uint32_t mTimeOn;
    uint32_t mTimeoutTicks;
};

#endif //#ifndef __ANALOG_INPUT_BUTTON_H__
