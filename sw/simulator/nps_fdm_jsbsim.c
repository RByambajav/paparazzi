#include <FGFDMExec.h>
#include <FGJSBBase.h>
#include <FGState.h>
#include <stdlib.h>
#include "nps_fdm.h"
#include "6dof.h"
#include "airframe.h"

using namespace JSBSim;

static void feed_jsbsim(double* commands);
static void fetch_state(void);
static void jsbsimvec_to_vec(DoubleVect3* vector, const FGColumnVector3* jsb_vector);
static void jsbsimloc_to_loc(EcefCoor_d* fdm_loc, FGLocation* location);
static void jsbsimquat_to_quat(DoubleQuat* ltp_to_body_quat, FGQuaternion* quat);
static void jsbsimvec_to_rate(DoubleRates* rate, const FGColumnVector3* jsb_vector);
static void init_jsbsim(double dt);
static void init_fdm_vars(void);

struct NpsFdm fdm;
FGFDMExec* FDMExec;

void nps_fdm_init(double dt) {

  init_jsbsim(dt);
  init_fdm_vars();
 
}

void nps_fdm_run_step(double* commands) {

  feed_jsbsim(commands);

  FDMExec->Run();

  // fetch_state();

}

static void feed_jsbsim(double* commands) {

  char buf[64];
  const char* names[] = NPS_ACTUATOR_NAMES;
  string property;

  int i;
  for (i=0; i<SERVOS_NB; i++) {
    sprintf(buf,"fcs/%s",names[i]);
    property = string(buf);
    FDMExec->GetPropertyManager()->SetDouble(property,commands[i]);
  }
}

static void fetch_state(void) {

  FGGroundReactions* ground_reactions;
  FGPropagate* propagate;
  FGPropagate::VehicleState* VState;

  ground_reactions = FDMExec->GetGroundReactions();
  propagate = FDMExec->GetPropagate();
  VState = propagate->GetVState();

  fdm.on_ground = ground_reactions->GetWOW();
  //jsbsimloc_to_vec(fdm.ecef_pos,&VState->vLocation);
  //jsbsimvec_to_vec(fdm.ecef_vel,&VState->vUVW);
  //jsbsimvec_to_vec(fdm.body_accel,&propagate->GetUVWdot());
  //jsbsimquat_to_vec(fdm.ltp_to_body_quat,&VState->vQtrn);
  //jsbsimvec_to_vec(fdm.body_rate,&VState->vPQR);
  //jsbsimvec_to_vec(fdm.body_rate_dot,&propagate->GetPQRdot());

}


static void init_jsbsim(double dt) {

  char buf[1024];
  string rootdir;
  JSBSim::FGState* State;
  
  sprintf(buf,"%s/conf/simulator/",getenv("PAPARAZZI_HOME"));
  rootdir = string(buf);
  FDMExec = new FGFDMExec();
  
  State = FDMExec->GetState();
  State->Setsim_time(0.);
  State->Setdt(dt);
  
  FDMExec->DisableOutput();
  FDMExec->SetDebugLevel(0); // No DEBUG messages
  
  if ( ! FDMExec->LoadModel( rootdir + "aircraft",
			     rootdir + "engine",
			     rootdir + "systems",
			     AIRFRAME_NAME,
			     false)){
#ifdef DEBUG
    cerr << "  JSBSim could not be started" << endl << endl;
#endif
    delete FDMExec;
    exit(-1);
  }
  
  JSBSim::FGInitialCondition *IC = FDMExec->GetIC();
  if ( ! IC->Load(NPS_INITIAL_CONDITITONS)) {
#ifdef DEBUG
    cerr << "Initialization unsuccessful" << endl;
#endif
    delete FDMExec;
    exit(-1);
  }

}

static void init_fdm_vars(void) {}
static void jsbsimvec_to_rate(DoubleRates* rate, const FGColumnVector3* jsb_vector) {}
static void jsbsimloc_to_loc(EcefCoor_d* fdm_loc, FGLocation* location){}
static void jsbsimquat_to_quat(DoubleQuat* ltp_to_body_quat, FGQuaternion* quat){}
static void jsbsimvec_to_vec(DoubleVect3* vector, const FGColumnVector3* jsb_vector) {}

