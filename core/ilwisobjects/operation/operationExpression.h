#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QList>
#include "Kernel_global.h"

namespace Ilwis {
    enum OperationType{otUnknown=0,otFunction=1, otCommand=2, otSelection=4};

class KERNELSHARED_EXPORT Parameter : public Identity{
public:

    Parameter();
    Parameter(const QString& name, const QString& value, IlwisTypes=itUNKNOWN);
    Parameter(const QString& value, IlwisTypes=itUNKNOWN);
    virtual ~Parameter();
    QString value() const;
    QString domain() const;
    void domain(const QString& dom);
    QString modifier(const QString &key) const;
    IlwisTypes valuetype() const;
    bool isEqual(const Parameter& parm) const;
    bool isValid() const;
    static IlwisTypes determineType(const QString &value) ;

private:
    QString _key;
    QString _value;
    std::map<QString,QString> _modifiers;
    IlwisTypes _type;
    QString _domain;
};

/*!
 The OperationExpression class a parser for fully defined expressions. This class will parse expressions of the type function and command and split it into its parts. The assumption is that the expression is already parsed to a level that all parts can not be parsed further.
 more complex expressions need first go to the calculator to be reduced to this level. Note that functions have a fixed order of parameters v = func(a,b) is different from b fun(b,a)
 */
class KERNELSHARED_EXPORT OperationExpression
{
public:
    /*!
     *  OperationExpression empty expression
     */
    OperationExpression();
    ~OperationExpression();
    /*!
     * Constructs an expression object of an textual expression.
     *
     *Expressions have either the form of a function which takes the form of output*={id}(paramteres*) or a command with the form {id} (paramters)*. The class will splt this in
     *ids, input parametrs and return parameters
     *
     * \param expr textual form of the expression
     * \param type enum marking the type of the expression, function or command.
     */
    OperationExpression(const QString& expr);
    /*!
     *  returns the parameter at a defined placed in either the input or the output
     * \param index rank order number of the parameters to be returned
     * \param in input parameter or output parameter
     * \return a parameter
     */
    Parameter parm(int index, bool in=true) const;
    /*!
     *  returns the parameter at a defined name in either the input or the output
     * \param key name of the parameters to be returned
     * \param toLower name will be in lowercase (or not)
     * \param in input parameter or output parameter
     * \return a parameter
     */

    Parameter parm(const QString key, bool toLower=true, bool in=true) const;
    /*!
     *  name returns the name of the command or function
     * \param toLower toLower name will be in lowercase (or not)
     * \return the function/command id
     */
    QString name(bool toLower=true) const;
    /*!
     *  parameterCount returns the number of input or outparameters
     * \param in input or output
     * \return count
     */
    int parameterCount(bool in=true) const;
    /*!
     *  isValid minimum condition for an expression to be valid, it must have a name
     *
     *  An expression might have no return types and no parameters but it always must have a name
     * \return true if there is a name
     */
    bool isValid() const;
    QUrl metaUrl(bool simple=true) const;
    void setExpression(const QString &e);
    bool matchesParameterCount(const QString &match, bool in=true) const;
private:
    QString _name;
    QList<Parameter> _inParameters;
    QList<Parameter> _outParameters;
    OperationType _type;
    QString _selection;

    void parseFunctionExpression(const QString &txt);
    void parseCommandExpression(const QString &expr);
    void parseSelectors(const QString& selectors);
    void specialExpressions(const QString &e);
};



}

#endif // EXPRESSION_H
