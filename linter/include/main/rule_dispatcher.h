#pragma once

#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <uhdm/vpi_user.h>

#include <array>
#include <filesystem>
#include <functional>
#include <string_view>

#include "rules/assertion_statement_atribute_instance.h"
#include "rules/assignment_pattern.h"
#include "rules/assignment_pattern_context.h"
#include "rules/assignment_pattern_values.h"
#include "rules/circular_inheritance.h"
#include "rules/class_variable_lifetime.h"
#include "rules/concatenation_multiplier.h"
#include "rules/covergroup_expression.h"
#include "rules/coverpoint_expression_type.h"
#include "rules/dpi_decl_string.h"
#include "rules/duplicate_class.h"
#include "rules/duplicate_constructor.h"
#include "rules/empty_assignment_pattern.h"
#include "rules/event_control_expression.h"
#include "rules/exponent_format_time_value.h"
#include "rules/extend_class.h"
#include "rules/extend_interface_class.h"
#include "rules/extern_constraint_undeclared.h"
#include "rules/extern_function_undeclared.h"
#include "rules/extern_task_undeclared.h"
#include "rules/fatal_rule.h"
#include "rules/foreach_loop_condition.h"
#include "rules/function_implementation_internal_return_type.h"
#include "rules/function_implementation_return_type.h"
#include "rules/function_implemention_scope.h"
#include "rules/hierarchical_interface_identifier.h"
#include "rules/implement_class.h"
#include "rules/implement_interface_class.h"
#include "rules/implicit_data_type.h"
#include "rules/incomplete_assignment_pattern.h"
#include "rules/inside_operator.h"
#include "rules/inside_operator_range.h"
#include "rules/logical_negation.h"
#include "rules/method_implementation_argument_type.h"
#include "rules/method_override_argument_name.h"
#include "rules/missing_for_loop_condition.h"
#include "rules/missing_for_loop_initialization.h"
#include "rules/missing_for_loop_step.h"
#include "rules/missing_function_implementation.h"
#include "rules/modport_import_export_port.h"
#include "rules/multiple_bins.h"
#include "rules/multiple_dot_star_connection.h"
#include "rules/nof_parameter_override.h"
#include "rules/parameter_dynamic_array.h"
#include "rules/parameter_override.h"
#include "rules/prototype_return_data_type.h"
#include "rules/repeat_in_sequence.h"
#include "rules/scalar_assignment_pattern.h"
#include "rules/select_in_event_control.h"
#include "rules/select_in_weight.h"
#include "rules/system_function_arguments.h"
#include "rules/target_unpacked_array_concatenation.h"
#include "rules/time_value.h"
#include "rules/type_casting.h"
#include "rules/void_cast_of_void_function.h"
#include "rules/wildcard_operator.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/null.h"

constexpr const char* DefaultConfigFileName = ".verihogg-lint";

namespace RuleInfo {
struct Rule {
  std::string_view idName;
  std::string_view description;
  std::string_view internalName;
  std::function<void(const SURELOG::FileContent*, SURELOG::ErrorContainer*,
                     SURELOG::SymbolTable*)>
      check;
};

struct GlobalRule {
  std::string_view idName;
  std::string_view description;
  std::string_view internalName;
  std::function<void(SURELOG::Design*, SURELOG::ErrorContainer*,
                     SURELOG::SymbolTable*)>
      check;
};

const auto allRules = std::to_array<Rule>({
    {.idName = "ASSERTION_STATEMENT_ATTRIBUTE_INSTANCE",
     .description = "Expecting attribute instance after block idNameentifier "
                    "for assertion",
     .internalName = "AssertionstatementAttributeInstance",
     .check = CheckAssertionStatementAttributeInstance},
    {.idName = "ASSIGNMENT_PATTERN",
     .description =
         "Expecting assignment pattern '{...}' instead of concatenation",
     .internalName = "AssignmentPattern",
     .check = CheckAssignmentPattern},
    {.idName = "ASSIGNMENT_PATTERN_CONTEXT",
     .description =
         "Assignment pattern not allowed outsidNamee assignment-like context",
     .internalName = "AssignmentPatternContext",
     .check = CheckAssignmentPatternContext},
    {.idName = "CIRCULAR_INHERITANCE",
     .description = "class extends itself",
     .internalName = "CircularInheritance",
     .check = CheckCircularInheritance},
    {.idName = "CLASS_VARIABLE_LIFETIME",
     .description = "'automatic' lifetime for class variable not allowed",
     .internalName = "ClassVariableLifetime",
     .check = CheckClassVariableLifetime},
    {.idName = "CONCATENATION_MULTIPLIER",
     .description = "Expecting constant expression as concatenation multiplier",
     .internalName = "ConcatenationMultiplier",
     .check = CheckConcatenationMultiplier},
    {.idName = "COVERPOINT_EXPRESSION_TYPE",
     .description = "Coverpoint expression should be of an integral data type",
     .internalName = "CovergroupExpression",
     .check = CheckCovergroupExpression},
    {.idName = "DPI_DECLARATION_STRING",
     .description = R"(Expecting "DPI" or "DPI-C")",
     .internalName = "CoverpointExpressionType",
     .check = CheckCoverpointExpressionType},
    {.idName = "DUPLICATE_CLASS",
     .description = "duplicate class, already declared",
     .internalName = "DpiDeclarationString",
     .check = CheckDpiDeclarationString},
    {.idName = "DUPLICATE_CONSTRUCTOR",
     .description = "duplicate constructor already declared",
     .internalName = "DuplicateClass",
     .check = CheckDuplicateClass},
    {.idName = "EMPTY_ASSIGNMENT_PATTERN",
     .description = "Empty assignment pattern '{}' not allowed",
     .internalName = "DuplicateConstructor",
     .check = CheckDuplicateConstructor},
    {.idName = "EXPONENT_FORMAT_TIME_VALUE",
     .description = "Unexpected exponent format for time value",
     .internalName = "EmptyAssignmentPattern",
     .check = CheckEmptyAssignmentPattern},
    {.idName = "EXTEND_CLASS",
     .description = "extending non existing class",
     .internalName = "ExponentFormatTimeValue",
     .check = CheckExponentFormatTimeValue},
    {.idName = "MODPORT_IMPORT_EXPORT_PORT",
     .description = "еxpected method name instead of interface signal name",
     .internalName = "ModportImportExportProt",
     .check = CheckModportImportExportPort},
    {.idName = "EVENT_CONTROL_EXPRESSION",
     .description = "еxpected singular data type for event control expression "
                    "instead of type",
     .internalName = "EventControlExpression",
     .check = CheckEventControlExpression},
    {.idName = "EXTEND_INTERFACE_CLASS",
     .description =
         "extending interface class by non-interface class not allowed",
     .internalName = "ExtendClass",
     .check = CheckExtendClass},
    {.idName = "EXTERN_CONSTRAINT_UNDECLARED",
     .description = "outer class constraint was not declared extern",
     .internalName = "ExtendInterfaceClass",
     .check = CheckExtendInterfaceClass},
    {.idName = "EXTERN_FUNCTION_UNDECLARED",
     .description = "outer class function was not declared extern",
     .internalName = "ExternConstraintUndeclared",
     .check = CheckExternConstraintUndeclared},
    {.idName = "EXTERN_TASK_UNDECLARED",
     .description = "outer class task was not declared extern",
     .internalName = "ExternFunctionUndeclared",
     .check = CheckExternFunctionUndeclared},
    {.idName = "EXTERN_TASK_UNDECLARED",
     .description = "outer class task was not declared extern",
     .internalName = "ExternTaskUndeclared",
     .check = CheckExternTaskUndeclared},
    {.idName = "FOREACH_LOOP_CONDITION",
     .description = "MultidNameimensional array select not allowed in foreach "
                    "loop condition",
     .internalName = "ForeachLoopCondition",
     .check = CheckForeachLoopCondition},

    {.idName = "HIERARCHICAL_INTERFACE_IDNAMEENTIFIER",
     .description = "Hierarchical interface idNameentifier not allowed",
     .internalName = "HierarchicalInterfaceIdentifier",
     .check = CheckHierarchicalInterfaceIdentifier},
    {.idName = "IMPLEMENT_CLASS",
     .description = "implementing non-interface class by class not allowed",
     .internalName = "ImplementClass",
     .check = CheckImplementClass},
    {.idName = "IMPLEMENT_INTERFACE_CLASS",
     .description = "implementing non existing interface class",
     .internalName = "ImplementInterfaceClass",
     .check = CheckImplementInterfaceClass},
    {.idName = "IMPLICIT_DATA_TYPE_IN_DECLARATION",
     .description =
         "Expecting net type (e.g. wire) or 'var' before implicit data type",
     .internalName = "ImplicitDataTypeInDeclaration",
     .check = CheckImplicitDataTypeInDeclaration},
    {.idName = "INSIDNAMEE_OPERATOR",
     .description = "'insidNamee' operator in constant expression not allowed",
     .internalName = "InsideOperator",
     .check = CheckInsideOperator},
    {.idName = "INSIDNAMEE_OPERATOR_RANGE",
     .description =
         "Expecting curly braces {} around 'insidNamee' operator range",
     .internalName = "InsideOperatorRange",
     .check = CheckInsideOperatorRange},
    {.idName = "MISSING_FOR_LOOP_CONDITION",
     .description = "'for' loop conditional expression required",
     .internalName = "MissingForLoopCondition",
     .check = CheckMissingForLoopCondition},
    {.idName = "MISSING_FOR_LOOP_INITIALIZATION",
     .description = "'for' loop variable initialization required",
     .internalName = "MissingForLoopInitialization",
     .check = CheckMissingForLoopInitialization},
    {.idName = "MISSING_FOR_LOOP_STEP",
     .description = "'for' loop step required",
     .internalName = "MissingForLoopStep",
     .check = CheckMissingForLoopStep},

    {.idName = "MISSING_TASK_IMPLEMENTATION",
     .description = "extern task is not implemented",
     .internalName = "MultipleBins",
     .check = CheckMultipleBins},
    {.idName = "MULTIPLE_DOT_STAR_CONNECTIONS",
     .description = "'.*' cannot appear more than once in the port list",
     .internalName = "MultipleDotStarConnections",
     .check = CheckMultipleDotStarConnections},

    {.idName = "PARAMETER_DYNAMIC_ARRAY",
     .description = "Fixed size required for parameter dimension",
     .internalName = "ParameterDynamicArray",
     .check = CheckParameterDynamicArray},
    {.idName = "PARAMETER_OVERRIDNAMEE",
     .description = "Expecting parentheses around parameter overridNamee",
     .internalName = "ParameterOverride",
     .check = CheckParameterOverride},
    {.idName = "PROTOTYPE_RETURN_DATA_TYPE",
     .description =
         "Expecting return data type or voidName for function prototype",
     .internalName = "PrototypeReturnDataType",
     .check = CheckPrototypeReturnDataType},
    {.idName = "REPETITION_IN_SEQUENCE",
     .description = "Goto '[-> and non-consecutive '[= operators not allowed",
     .internalName = "RepetitionInSequence",
     .check = CheckRepetitionInSequence},
    {.idName = "SCALAR_ASSIGNMENT_PATTERN",
     .description = "Variable of 1-bit scalar type not allowed as assignment "
                    "pattern target",
     .internalName = "ScalarAssignmentPattern",
     .check = CheckScalarAssignmentPattern},
    {.idName = "IncompleteAssignmentPattern",
     .check = CheckIncompleteAssignmentPattern},
    {.idName = "AssignmentPatternValues",
     .check = CheckAssignmentPatternValues},
    {.idName = "SELECT_IN_EVENT_CONTROL",
     .description = "Select in event control not allowed",
     .internalName = "SelectInEventControl",
     .check = CheckSelectInEventControl},
    {.idName = "SELECT_IN_WEIGHT",
     .description = "Select in weight specification not allowed",
     .internalName = "SelectInWeight",
     .check = CheckSelectInWeight},
    {.idName = "SYSTEM_FUNCTION_ARGUMENTS",
     .description = "Maximum number of arguments for system function exceeded",
     .internalName = "SystemFunctionArguments",
     .check = CheckSystemFunctionArguments},
    {.idName = "TARGET_UNPACKED_ARRAY_CONCATENATION",
     .description =
         "Unpacked array concatenation not allowed as target expression",
     .internalName = "TargetUnpackedArrayConcatenation",
     .check = CheckTargetUnpackedArrayConcatenation},
    {.idName = "TIME_VALUE",
     .description = "Unexpected white space between number and time value",
     .internalName = "TimeValue",
     .check = CheckTimeValue},
    {.idName = "TYPE_CASTING",
     .description = "Expecting tick before type casting expression",
     .internalName = "TypeCasting",
     .check = CheckTypeCasting},
    {.idName = "WILDCARD_EQUALITY_OPERATOR",
     .description = "Expecting wildcard operator '==?' instead of '=?='",
     .internalName = "WildcardOperator",
     .check = CheckWildcardOperators},
    {.idName = "VOID_CAST_OF_VOID_FUNCTION",
     .description = "void cast of void function not allowed",
     .internalName = "VoidCastOfVoidFunction",
     .check = CheckVoidCastOfVoidFunction},
    {.idName = "LOGICAL_NEGATION",
     .description = "Operand of type not allowed with logical negation (use == "
                    "null instead)",
     .internalName = "LogicalNegation",
     .check = CheckLogicalNegation},
});

constexpr size_t AllRulesSize = allRules.size();

const auto globalRules = std::to_array<GlobalRule>({
    {.idName = "NOF_PARAMETER_OVERRIDNAMEES",
     .description =
         "Expected # parameter overridNamees, found #module; endmodule",
     .internalName = "NofParameterOverrides",
     .check = CheckNofParameterOverrides},
    {.idName = "MISSING_FUNCTION_IMPLEMENTATION",
     .description = "extern function is not implemented",
     .internalName = "MissingFunctionImplementation",
     .check = CheckMissingFunctionImplementation},
    {.idName = "FUNCTION_IMPLEMENTATION_SCOPE",
     .description = "extern task implemented outsidNamee of its class scope",
     .internalName = "FuctionImplementationScope",
     .check = CheckFuncImplScope},
    {.idName = "METHOD_OVERRIDE_ARGUMENT_NAME",
     .description = "argument name of method does not match of override",
     .internalName = "MethodOverrideArgumentName",
     .check = CheckMethodOverrideArgumentName},
    {.idName = "FUNCTION_IMPLEMENTATION_RETURN_TYPE",
     .description =
         "return type of function must be the same as prototype return type",
     .internalName = "FunctionImplementationReturnType",
     .check = CheckFunctionImplementationReturnType},
    {.idName = "FUNCTION_IMPLEMENTATION_INTERNAL_RETURN_TYPE",
     .description = "Internal return type for the implementation of extern "
                    "method requires scope resolution",
     .internalName = "FunctionImplementationInternalReturnType",
     .check = CheckFunctionImplementationInternalReturnType},
    {.idName = "METHOD_IMPLEMENTATION_ARGUMENT_TYPE",
     .description = "Argument type of method must be the same as prototype "
                    "argument type (non-standard use of type alias)",
     .internalName = "MethodImplementationArgumentType",
     .check = CheckMethodImplementationArgumentType},
});

constexpr size_t AllGlobalRulesSize = globalRules.size();

constexpr size_t TotalRuleCount = AllRulesSize + AllGlobalRulesSize;
}  // namespace RuleInfo

void RunAllRulesOnDesign(SURELOG::Design* design, const vpiHandle& uhdmDesign,
                         SURELOG::ErrorContainer* errors,
                         SURELOG::SymbolTable* symbols,
                         const std::filesystem::path& configFile);
