#pragma once

#include <cstddef>
#include <type_traits>	// c++11 header file

// for type-safe check
template< typename StatesT,
		  StatesT StateInitParam,
		  StatesT StateTerminateParam >
struct StatesCorrect {

	enum {

		// only work on c++11, use this gcc flag to compile: --std=c++0x
        value = ( std::is_enum<StatesT>::value &&
				  StateInitParam == 0 &&
				  StateTerminateParam >= StateInitParam )
    };
};

// SMC: state machine components, holds three basic definitions of FSM
template< typename StatesT,
		  typename TransitionsType,
		  StatesT StateInitParam,
		  StatesT StateTerminateParam,
		  typename SharedDataType,
		  bool GuardTrait =
			static_cast<bool>( StatesCorrect< StatesT,
							   StateInitParam,
							   StateTerminateParam >::value )
		>
struct SMC {};

template< typename StatesT,
		  typename TransitionsType,
		  StatesT StateInitParam,
		  StatesT StateTerminateParam,
		  typename SharedDataType
		>
struct SMC< StatesT,
			TransitionsType,
			StateInitParam,
			StateTerminateParam,
			SharedDataType,
			true >
{
	typedef StatesT			States;
    typedef TransitionsType Transitions;
	typedef SharedDataType	SharedData;
    enum {  InitState 	    = StateInitParam };
    enum {  TerminateState  = StateTerminateParam };
};

// base transition interface
template <typename SMC>
struct TransitionsI : SMC::Transitions {

    typedef typename SMC::States		States;
    typedef typename SMC::SharedData SharedData;

    States&		state;
    SharedData&	data;
    TransitionsI(States& state, SharedData& data) : state(state), data(data) {}
};

// transition implement
template< typename SMC,
		  typename SMC::States stateParam
		>
struct TransitionImpl : TransitionsI<SMC> {

    TransitionImpl(typename SMC::States& state, typename SMC::SharedData& data)
		: TransitionsI<SMC>(state, data) {}
};

// the FSM declaration
template<typename SMC>
class FSM {

private:

	// must exist in SMC
    typedef typename SMC::States 	  States;
    typedef typename SMC::SharedData  SharedData;
    typedef typename SMC::Transitions Transitions;

	// important constant
    enum { InitState 	  = SMC::InitState };
    enum { TerminateState = SMC::TerminateState };
    enum { Size = static_cast<size_t>(
					static_cast<size_t>( TerminateState )
					- static_cast<size_t>( InitState ) + 1
				  )
	};

	// --- BEGIN ---
	// code generator for state handler (static safe-type)
    struct BaseHandler {

		States			state;
        SharedData		data;
        Transitions*	m_interfaces[ static_cast<size_t>( Size ) ];

        BaseHandler() : state( static_cast<States>( InitState ) ),
							 data( SharedData() ) {}
    };

    typedef int DES;	// DES: dummy explicit specialization
    template <States stateParam, class Dummy = DES>
    struct Handler : Handler< static_cast<States>( static_cast<size_t>( stateParam ) - 1 ) >
    {
        TransitionImpl< SMC, static_cast<States>( stateParam ) > m_interface;
        Handler() : m_interface(BaseHandler::state, BaseHandler::data) {

            BaseHandler::m_interfaces[ static_cast<size_t>( stateParam ) ] = &m_interface;
        }
    };

    template <typename CDES>	// CDES: class of dummy explicit specialization
    struct Handler<static_cast<States>(0), CDES> : BaseHandler {

        TransitionImpl<SMC, static_cast<States>(0)> m_interface;
        Handler() : m_interface(BaseHandler::state, BaseHandler::data) {

            BaseHandler::m_interfaces[0] = &m_interface;
        }
    };

    // when compiler saw this instance, it will automaticity specialization all the template above
    Handler<static_cast<States>(TerminateState), DES>	_instance;

	// recursive inheritance explanation:
	// Handler<TerminateState, DES> ->
	// Handler<TerminateState - 1, DES> ->
    // Handler<(TerminateState - 1) - 1, DES> ->
    // Handler<((TerminateState - 1) - 1) - 1, DES> ->
	// ... ->
	// Handler<0, DES> ->
	// BaseHandler
	// --- END ---

	States& state;

public:

    Transitions* operator->() {

        return _instance.m_interfaces[ static_cast<size_t>( _instance.state ) ];
    }

    SharedData& data;
    States GetState() const { return state; } // inline

    FSM() : _instance(),
    		state(_instance.state),
    		data(_instance.data) {}

	// 1. construct references to Handler
	FSM(const FSM& lhs) : data(_instance.data), state(_instance.state) {

		// 2. copy after reference consturction the values from lhs
		if ( this != &lhs ) {

			data  = lhs.data;	// data must be copy constructable
			state = lhs.state;
		}
	}
	const FSM& operator=(const FSM& lhs) {

		// copy after reference consturction the values from lhs
		if (this != &lhs) {

			data  = lhs.data;	// data must be copy constructable
			state = lhs.state;
		}
		return *this;
	}
};

// helpful marco
#define BEGIN_TRANSITION_IMPL(StateMachineType , State) template<> \
			struct TransitionImpl<StateMachineType, State> \
				: TransitionsI<StateMachineType> { \
					TransitionImpl (States &state, SharedData &data) \
						: TransitionsI<StateMachineType>(state, data) {}
#define END_TRANSITION_IMPL };
