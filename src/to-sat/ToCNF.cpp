// -*- c++ -*-
/********************************************************************
 * AUTHORS: Mike Katelman
 *
 * BEGIN DATE: November, 2005
 *
 * LICENSE: Please view LICENSE file in the home dir of this Program
 ********************************************************************/

#include "../AST/AST.h"
#include "../STPManager/STPManager.h"
#include "../sat/sat.h"
#include "ToCNF.h"

namespace BEEV
{
  //############################################################
  //############################################################
  void DeleteClauseList(ClauseList *cllp)
  {
    ClauseList::const_iterator iend = cllp->end();
    for (ClauseList::const_iterator i = cllp->begin(); i < iend; i++)
      {
        delete *i;
      }
    delete cllp;
  } //End of DeleteClauseList
  
  bool CNFMgr::isAtom(const ASTNode& varphi)
  {
    bool result;
    
    Kind k = varphi.GetKind();
    switch (k)
      {
      case TRUE:
        {
          result = true;
          break;
        }
      case FALSE:
        {
          result = true;
          break;
        }
      case SYMBOL:
        {
          result = true;
          break;
        }
      case BVCONST:
        {
          result = true;
          break;
        }
      default:
        {
          result = false;
          break;
        }
      }
    
    return result;
  } //End of isAtom()
  
  bool CNFMgr::isPred(const ASTNode& varphi)
  {
    bool result;
    
    Kind k = varphi.GetKind();
    switch (k)
      {
      case BVLT:
        {
          result = true;
          break;
        }
      case BVLE:
        {
          result = true;
          break;
        }
      case BVGT:
        {
          result = true;
          break;
        }
      case BVGE:
        {
          result = true;
          break;
        }
      case BVSLT:
        {
          result = true;
          break;
        }
      case BVSLE:
        {
          result = true;
          break;
        }
      case BVSGT:
        {
          result = true;
          break;
        }
      case BVSGE:
        {
          result = true;
          break;
        }
      case EQ:
        {
          result = true;
          break;
        }
      default:
        {
          result = false;
          break;
        }
      }
    
    return result;
  } //End of isPred()

  bool CNFMgr::isITE(const ASTNode& varphi)
  {
    bool result;
    
    Kind k = varphi.GetKind();
    switch (k)
      {
      case ITE:
        {
          result = true;
          break;
        }
      default:
        {
          result = false;
          break;
        }
      }
    
    return result;
  } //End of isITE()

  bool CNFMgr::onChildDoPos(const ASTNode& varphi, unsigned int idx)
  {
    bool result = true;
    
    Kind k = varphi.GetKind();
    switch (k)
      {
      case NOT:
        {
          result = false;
          break;
        }
      case NAND:
        {
          result = false;
          break;
        }
      case NOR:
        {
          result = false;
          break;
        }
      case IMPLIES:
        {
          if (idx == 0)
            {
              result = false;
            }
          break;
        }
      default:
        {
          break;
        }
      }
    
    return result;
  } //End of onChildDoPos()

  bool CNFMgr::onChildDoNeg(const ASTNode& varphi, unsigned int idx)
  {
    bool result = false;

    Kind k = varphi.GetKind();
    switch (k)
      {
      case NOT:
        {
          result = true;
          break;
        }
      case NAND:
        {
          result = true;
          break;
        }
      case NOR:
        {
          result = true;
          break;
        }
      case XOR:
        {
          result = true;
          break;
        }
      case IFF:
        {
          result = true;
          break;
        }
      case IMPLIES:
        {
          if (idx == 0)
            {
              result = true;
            }
          break;
        }
      case ITE:
        {
          if (idx == 0)
            {
              result = true;
            }
          break;
        }
      default:
        {
          break;
        }
      }
    
    return result;
  } //End of onChildDoNeg()
  
  //########################################
  //########################################
  //utilities for control bits.

  void CNFMgr::initializeCNFInfo(CNFInfo& x)
  {
    x.control = 0;
    x.clausespos = NULL;
    x.clausesneg = NULL;
  } //End of initlializeCNFInfo()
  
  void CNFMgr::incrementSharesPos(CNFInfo& x)
  {
    x.control += ((x.control & 3) < 2) ? 1 : 0;
  } //End of incrementSharesPos()
  
  int CNFMgr::sharesPos(CNFInfo& x)
  {
    return (x.control & 3);
  } //End of sharesPos()
  
  void CNFMgr::incrementSharesNeg(CNFInfo& x)
  {
    x.control += ((x.control & 12) < 8) ? 4 : 0;
  } //End of incrementSharesNeg()
  
  int CNFMgr::sharesNeg(CNFInfo& x)
  {
    return ((x.control & 12) >> 2);
  } //End of sharesNeg()

  void CNFMgr::setControlBit(CNFInfo& x, unsigned int idx)
  {
    x.control |= (1 << idx);
  } //End of setControlBit()
  
  bool CNFMgr::getControlBit(CNFInfo& x, unsigned int idx)
  {
    bool result = false;
    
    if (x.control & (1 << idx))
      { 
        result = true;
      }

    return result;
  } //End of getControlBit()

  void CNFMgr::setIsTerm(CNFInfo& x)
  {
    setControlBit(x, 4);
  } //End of setIsTerm()

  bool CNFMgr::isTerm(CNFInfo& x)
  {
    return getControlBit(x, 4);
  }

  void CNFMgr::setDoRenamePos(CNFInfo& x)
  {
    setControlBit(x, 5);
  }

  bool CNFMgr::doRenamePos(CNFInfo& x)
  {
    return getControlBit(x, 5);
  }

  void CNFMgr::setWasRenamedPos(CNFInfo& x)
  {
    setControlBit(x, 6);
  }

  bool CNFMgr::wasRenamedPos(CNFInfo& x)
  {
    return getControlBit(x, 6);
  }

  void CNFMgr::setDoRenameNeg(CNFInfo& x)
  {
    setControlBit(x, 7);
  }

  bool CNFMgr::doRenameNeg(CNFInfo& x)
  {
    return getControlBit(x, 7);
  }

  void CNFMgr::setWasRenamedNeg(CNFInfo& x)
  {
    setControlBit(x, 8);
  }

  bool CNFMgr::wasRenamedNeg(CNFInfo& x)
  {
    return getControlBit(x, 8);
  }

  void CNFMgr::setDoSibRenamingPos(CNFInfo& x)
  {
    setControlBit(x, 9);
  }

  bool CNFMgr::doSibRenamingPos(CNFInfo& x)
  {
    return getControlBit(x, 9);
  }

  void CNFMgr::setDoSibRenamingNeg(CNFInfo& x)
  {
    setControlBit(x, 10);
  }

  bool CNFMgr::doSibRenamingNeg(CNFInfo& x)
  {
    return getControlBit(x, 10);
  }

  void CNFMgr::setWasVisited(CNFInfo& x)
  {
    setControlBit(x, 11);
  }

  bool CNFMgr::wasVisited(CNFInfo& x)
  {
    return getControlBit(x, 11);
  }
  
  //########################################
  //########################################
  //utilities for clause sets
  

  ClauseList* CNFMgr::COPY(const ClauseList& varphi)
  {
    ClauseList* psi = new ClauseList();
    
    ClauseList::const_iterator it = varphi.begin();
    for (; it != varphi.end(); it++)
      {
        psi->push_back(new vector<const ASTNode*> (**it));
      }
    
    return psi;
  } //End of COPY()
  
  ClauseList* CNFMgr::SINGLETON(const ASTNode& varphi)
  {
    ASTNode* copy = ASTNodeToASTNodePtr(varphi);
    
    ClausePtr clause = new vector<const ASTNode*> ();
    clause->push_back(copy);
    
    ClauseList* psi = new ClauseList();
    psi->push_back(clause);
    return psi;
  } //End of SINGLETON()

  static ASTNode GetNodeFrom_SINGLETON(ClauseList *cl)
  {
    ClausePtr c = (*cl)[0];
    const ASTNode * a = (*c)[0];
    return *a;
  }

  ClauseList* CNFMgr::UNION(const ClauseList& varphi1, 
                            const ClauseList& varphi2)
  {    
    ClauseList* psi1 = COPY(varphi1);
    ClauseList* psi2 = COPY(varphi2);
    psi1->insert(psi1->end(), psi2->begin(), psi2->end());
    delete psi2;
    
    return psi1;    
  } //End of UNION()

  void CNFMgr::INPLACE_UNION(ClauseList* varphi1, 
                             const ClauseList& varphi2)
  {    
    ClauseList* psi2 = COPY(varphi2);
    varphi1->insert(varphi1->end(), psi2->begin(), psi2->end());
    delete psi2;
  } //End of INPLACE_UNION()

  void CNFMgr::NOCOPY_INPLACE_UNION(ClauseList* varphi1, 
                                    ClauseList* varphi2)
  {    
    varphi1->insert(varphi1->end(), varphi2->begin(), varphi2->end());
    delete varphi2;
  } //End of NOCOPY_INPLACE_UNION

  ClauseList* CNFMgr::PRODUCT(const ClauseList& varphi1, 
                              const ClauseList& varphi2)
  {    
    ClauseList* psi = new ClauseList();
    psi->reserve(varphi1.size() * varphi2.size());
    
    ClauseList::const_iterator it1 = varphi1.begin();
    for (; it1 != varphi1.end(); it1++)
      {
        ClausePtr clause1 = *it1;
        ClauseList::const_iterator it2 = varphi2.begin();
        for (; it2 != varphi2.end(); it2++)
          {
            ClausePtr clause2 = *it2;
            ClausePtr clause = new vector<const ASTNode*> ();
            clause->reserve(clause1->size() + clause2->size());
            clause->insert(clause->end(), clause1->begin(), clause1->end());
            clause->insert(clause->end(), clause2->begin(), clause2->end());
            psi->push_back(clause);
          }
      }
    
    return psi;
  } //End of Product

  //########################################
  //########################################
  //prep. for cnf conversion

  void CNFMgr::scanFormula(const ASTNode& varphi, bool isPos)
  {    
    CNFInfo* x;
    
    //########################################
    // step 1, get the info associated with this node
    //########################################
    
    if (info.find(varphi) == info.end())
      {
        x = new CNFInfo();
        initializeCNFInfo(*x);
        info[varphi] = x;
      }
    else
      {
        x = info[varphi];
      }
    
    //########################################
    // step 2, we only need to know if shares >= 2
    //########################################
    
    if (isPos && sharesPos(*x) == 2)
      {
        return;
      }
    
    if (!isPos && sharesNeg(*x) == 2)
      {
        return;
      }
    
    //########################################
    // step 3, set appropriate information fields
    //########################################
    
    if (isPos)
      {
        incrementSharesPos(*x);
      }
    
    if (!isPos)
      {
        incrementSharesNeg(*x);
      }
    
    //########################################
    // step 4, recurse over children
    //########################################
    
    if (isAtom(varphi))
      {
        return;
      }
    else if (isPred(varphi))
      {
        for (unsigned int i = 0; i < varphi.GetChildren().size(); i++)
          {
            scanTerm(varphi[i]);
          }
      }
    else
      {
        for (unsigned int i = 0; i < varphi.GetChildren().size(); i++)
          {
            if (onChildDoPos(varphi, i))
              {
                scanFormula(varphi[i], isPos);
              }
            if (onChildDoNeg(varphi, i))
              {
                scanFormula(varphi[i], !isPos);
              }
          }
      }
    
  } //End of ScanFormula()

  void CNFMgr::scanTerm(const ASTNode& varphi)
  {    
    CNFInfo* x;
    
    //########################################
    // step 1, get the info associated with this node
    //########################################
    
    if (info.find(varphi) == info.end())
      {
        x = new CNFInfo();
        initializeCNFInfo(*x);
        info[varphi] = x;
      }
    else
      {
        x = info[varphi];
      }
    
    //########################################
    // step 2, need two hits because of term ITEs.
    //########################################
    
    if (sharesPos(*x) == 2)
      {
        return;
      }
    
    //########################################
    // step 3, set appropriate data fields, always rename
    // term ITEs
    //########################################
    
    incrementSharesPos(*x);
    setIsTerm(*x);
    
    //########################################
    // step 4, recurse over children
    //########################################
    
    if (isAtom(varphi))
      {
        return;
      }
    else if (isITE(varphi))
      {
        scanFormula(varphi[0], true);
        scanFormula(varphi[0], false);
        scanTerm(varphi[1]);
        scanTerm(varphi[2]);
      }
    else
      {
        for (unsigned int i = 0; i < varphi.GetChildren().size(); i++)
          {
            scanTerm(varphi[i]);
          }
      }
  }//End of scanterm()
  
  //########################################
  //########################################
  // main cnf conversion function

  void CNFMgr::convertFormulaToCNF(const ASTNode& varphi, ClauseList* defs)
  {    
    CNFInfo* x = info[varphi];
    
    //########################################
    // divert to special case if term (word-level cnf)
    
    if (isTerm(*x))
      {
        convertTermForCNF(varphi, defs);
        setWasVisited(*x);
        return;
      }
    
    //########################################
    // do work
    
    if (sharesPos(*x) > 0 && !wasVisited(*x))
      {
        convertFormulaToCNFPosCases(varphi, defs);
      }
    
    if (x->clausespos != NULL && x->clausespos->size() > 1)
      {
        if (doSibRenamingPos(*x) || sharesPos(*x) > 1)
          {
            doRenamingPos(varphi, defs);
          }
      }
    
    if (sharesNeg(*x) > 0 && !wasVisited(*x))
      {
        convertFormulaToCNFNegCases(varphi, defs);
      }
    
    if (x->clausesneg != NULL && x->clausesneg->size() > 1)
      {
        if (doSibRenamingNeg(*x) || sharesNeg(*x) > 1)
          {
            doRenamingNeg(varphi, defs);
          }
      }
    
    //########################################
    //mark that we've already done the hard work
    
    setWasVisited(*x);
  } //End of convertFormulaToCNF()

  void CNFMgr::convertTermForCNF(const ASTNode& varphi, ClauseList* defs)
  {    
    CNFInfo* x = info[varphi];
    
    //########################################
    // step 1, done if we've already visited
    //########################################
    
    if (x->termforcnf != NULL)
      {
        return;
      }
    
    //########################################
    // step 2, ITE's always get renamed
    //########################################
    
    if (isITE(varphi))
      {
        x->termforcnf = doRenameITE(varphi, defs);
        reduceMemoryFootprintPos(varphi[0]);
        reduceMemoryFootprintNeg(varphi[0]);
        
      }
    else if (isAtom(varphi))
      {
        x->termforcnf = ASTNodeToASTNodePtr(varphi);
      }
    else
      { 
        ASTVec psis;
        ASTVec::const_iterator it = varphi.GetChildren().begin();
        for (; it != varphi.GetChildren().end(); it++)
          {
            convertTermForCNF(*it, defs);
            psis.push_back(*(info[*it]->termforcnf));
          }
        
        ASTNode psi = bm->CreateNode(varphi.GetKind(), psis);
        psi.SetValueWidth(varphi.GetValueWidth());
        psi.SetIndexWidth(varphi.GetIndexWidth());
        x->termforcnf = ASTNodeToASTNodePtr(psi);
      }
  } //End of convertTermForCNF()

  //########################################
  //########################################
  // functions for renaming nodes during cnf conversion

  ASTNode* CNFMgr::doRenameITE(const ASTNode& varphi, ClauseList* defs)
  {
    ASTNode psi;
    
    //########################################
    // step 1, old "RepLit" code
    //########################################
    
    ostringstream oss;
    oss << "cnf" << "{" << varphi.GetNodeNum() << "}";
    psi = bm->CreateSymbol(oss.str().c_str());
    
    //########################################
    // step 2, set widths appropriately
    //########################################
    
    psi.SetValueWidth(varphi.GetValueWidth());
    psi.SetIndexWidth(varphi.GetIndexWidth());
    
    //########################################
    // step 3, recurse over children
    //########################################
    
    convertFormulaToCNF(varphi[0], defs);
    convertTermForCNF(varphi[1], defs);
    ASTNode t1 = *(info[varphi[1]]->termforcnf);
    convertTermForCNF(varphi[2], defs);
    ASTNode t2 = *(info[varphi[2]]->termforcnf);
    
    //########################################
    // step 4, add def clauses
    //########################################
    
    ClauseList* cl1 = SINGLETON(bm->CreateNode(EQ, psi, t1));
    ClauseList* cl2 = PRODUCT(*(info[varphi[0]]->clausesneg), *cl1);
    DELETE(cl1);
    defs->insert(defs->end(), cl2->begin(), cl2->end());
    
    ClauseList* cl3 = SINGLETON(bm->CreateNode(EQ, psi, t2));
    ClauseList* cl4 = PRODUCT(*(info[varphi[0]]->clausespos), *cl3);
    DELETE(cl3);
    defs->insert(defs->end(), cl4->begin(), cl4->end());
    
    return ASTNodeToASTNodePtr(psi);
  }//End of doRenameITE()
  
  void CNFMgr::doRenamingPos(const ASTNode& varphi, ClauseList* defs)
  {
    CNFInfo* x = info[varphi];
    
    //########################################
    // step 1, calc new variable
    //########################################
    
    ostringstream oss;
    oss << "cnf" << "{" << varphi.GetNodeNum() << "}";
    ASTNode psi = bm->CreateSymbol(oss.str().c_str());
    
    //########################################
    // step 2, add defs
    //########################################
    
    ClauseList* cl1;
    cl1 = SINGLETON(bm->CreateNode(NOT, psi));
    ClauseList* cl2 = PRODUCT(*(info[varphi]->clausespos), *cl1);
    defs->insert(defs->end(), cl2->begin(), cl2->end());
    DELETE(info[varphi]->clausespos);
    DELETE(cl1);
    delete cl2;
    
    //########################################
    // step 3, update info[varphi]
    //########################################
    
    x->clausespos = SINGLETON(psi);
    setWasRenamedPos(*x);
  }//End of doRenamingPos
  

  void CNFMgr::doRenamingPosXor(const ASTNode& varphi)
  {
    CNFInfo* x = info[varphi];
    
    //########################################
    // step 1, calc new variable
    //########################################
    
    ostringstream oss;
    oss << "cnf" << "{" << varphi.GetNodeNum() << "}";
    ASTNode psi = bm->CreateSymbol(oss.str().c_str());
    
    //########################################
    // step 2, add defs
    //########################################
    
    //     ClauseList* cl1;
    //     cl1 = SINGLETON(bm->CreateNode(NOT, psi));
    //     ClauseList* cl2 = PRODUCT(*(info[varphi]->clausespos), *cl1);
    //     defs->insert(defs->end(), cl2->begin(), cl2->end());
    //     DELETE(info[varphi]->clausespos);
    //     DELETE(cl1);
    //     delete cl2;
    
    //########################################
    // step 3, update info[varphi]
    //########################################
    
    x->clausespos = SINGLETON(psi);
    setWasRenamedPos(*x);
  }//End of doRenamingPos
  

  void CNFMgr::doRenamingNeg(const ASTNode& varphi, ClauseList* defs)
  {
    CNFInfo* x = info[varphi];
    
    //########################################
    // step 2, calc new variable
    //########################################
    
    ostringstream oss;
    oss << "cnf" << "{" << varphi.GetNodeNum() << "}";
    ASTNode psi = bm->CreateSymbol(oss.str().c_str());
    
    //########################################
    // step 3, add defs
    //########################################
    
    ClauseList* cl1;
    cl1 = SINGLETON(psi);
    ClauseList* cl2 = PRODUCT(*(info[varphi]->clausesneg), *cl1);
    defs->insert(defs->end(), cl2->begin(), cl2->end());
    DELETE(info[varphi]->clausesneg);
    DELETE(cl1);
    delete cl2;
    
    //########################################
    // step 4, update info[varphi]
    //########################################
    
    x->clausesneg = SINGLETON(bm->CreateNode(NOT, psi));
    setWasRenamedNeg(*x);    
  } //End of doRenamingNeg()

  //########################################
  //########################################
  //main switch for individual cnf conversion cases

  void CNFMgr::convertFormulaToCNFPosCases(const ASTNode& varphi, 
                                           ClauseList* defs)
  {
    if (isPred(varphi))
      {
        convertFormulaToCNFPosPred(varphi, defs);
        return;
      }
    
    Kind k = varphi.GetKind();
    switch (k)
      {
      case FALSE:
        {
          convertFormulaToCNFPosFALSE(varphi, defs);
          break;
        }
      case TRUE:
        {
          convertFormulaToCNFPosTRUE(varphi, defs);
          break;
        }
      case BVGETBIT:
        {
          convertFormulaToCNFPosBVGETBIT(varphi, defs);
          break;
        }
      case SYMBOL:
        {
          convertFormulaToCNFPosSYMBOL(varphi, defs);
          break;
        }
      case NOT:
        {
          convertFormulaToCNFPosNOT(varphi, defs);
          break;
        }
      case AND:
        {
          convertFormulaToCNFPosAND(varphi, defs);
          break;
        }
      case NAND:
        {
          convertFormulaToCNFPosNAND(varphi, defs);
          break;
        }
      case OR:
        {
          convertFormulaToCNFPosOR(varphi, defs);
          break;
        }
      case NOR:
        {
          convertFormulaToCNFPosNOR(varphi, defs);
          break;
        }
      case XOR:
        {
          convertFormulaToCNFPosXOR(varphi, defs);
          break;
        }
      case IMPLIES:
        {
          convertFormulaToCNFPosIMPLIES(varphi, defs);
          break;
        }
      case ITE:
        {
          convertFormulaToCNFPosITE(varphi, defs);
          break;
        }
      default:
        {
          fprintf(stderr, "convertFormulaToCNFPosCases: "\
                  "doesn't handle kind %d\n", k);
          FatalError("");
        }
      }
  } //End of convertFormulaToCNFPosCases()

  void CNFMgr::convertFormulaToCNFNegCases(const ASTNode& varphi, 
                                           ClauseList* defs)
  {

    if (isPred(varphi))
      {
        convertFormulaToCNFNegPred(varphi, defs);
        return;
      }

    Kind k = varphi.GetKind();
    switch (k)
      {
      case FALSE:
        {
          convertFormulaToCNFNegFALSE(varphi, defs);
          break;
        }
      case TRUE:
        {
          convertFormulaToCNFNegTRUE(varphi, defs);
          break;
        }
      case BVGETBIT:
        {
          convertFormulaToCNFNegBVGETBIT(varphi, defs);
          break;
        }
      case SYMBOL:
        {
          convertFormulaToCNFNegSYMBOL(varphi, defs);
          break;
        }
      case NOT:
        {
          convertFormulaToCNFNegNOT(varphi, defs);
          break;
        }
      case AND:
        {
          convertFormulaToCNFNegAND(varphi, defs);
          break;
        }
      case NAND:
        {
          convertFormulaToCNFNegNAND(varphi, defs);
          break;
        }
      case OR:
        {
          convertFormulaToCNFNegOR(varphi, defs);
          break;
        }
      case NOR:
        {
          convertFormulaToCNFNegNOR(varphi, defs);
          break;
        }
      case XOR:
        {
          convertFormulaToCNFNegXOR(varphi, defs);
          break;
        }
      case IMPLIES:
        {
          convertFormulaToCNFNegIMPLIES(varphi, defs);
          break;
        }
      case ITE:
        {
          convertFormulaToCNFNegITE(varphi, defs);
          break;
        }
      default:
        {
          fprintf(stderr, "convertFormulaToCNFNegCases: "\
                  "doesn't handle kind %d\n", k);
          FatalError("");
        }
      }
  } //convertFormulaToCNFNegCases()

  //########################################
  //########################################
  // individual cnf conversion cases

  void CNFMgr::convertFormulaToCNFPosPred(const ASTNode& varphi,
                                          ClauseList* defs)
  {
    ASTVec psis;

    ASTVec::const_iterator it = varphi.GetChildren().begin();
    for (; it != varphi.GetChildren().end(); it++)
      {
        convertTermForCNF(*it, defs);
        psis.push_back(*(info[*it]->termforcnf));
      }

    info[varphi]->clausespos = 
      SINGLETON(bm->CreateNode(varphi.GetKind(), psis));
  } //End of convertFormulaToCNFPosPred()

  void CNFMgr::convertFormulaToCNFPosFALSE(const ASTNode& varphi,
                                           ClauseList* defs)
  {
    ASTNode dummy_false_var = 
      bm->CreateNode(NOT, bm->CreateSymbol("*TrueDummy*"));
    info[varphi]->clausespos = SINGLETON(dummy_false_var);
  } //End of convertFormulaToCNFPosFALSE()

  void CNFMgr::convertFormulaToCNFPosTRUE(const ASTNode& varphi, 
                                          ClauseList* defs)
  {
    ASTNode dummy_true_var = bm->CreateSymbol("*TrueDummy*");
    info[varphi]->clausespos = SINGLETON(dummy_true_var);
  } //End of convertFormulaToCNFPosTRUE

  void CNFMgr::convertFormulaToCNFPosBVGETBIT(const ASTNode& varphi, 
                                              ClauseList* defs)
  {
    info[varphi]->clausespos = SINGLETON(varphi);
  }//End of convertFormulaToCNFPosBVGETBIT()

  void CNFMgr::convertFormulaToCNFPosSYMBOL(const ASTNode& varphi, 
                                            ClauseList* defs)
  {
    info[varphi]->clausespos = SINGLETON(varphi);
  } //End of convertFormulaToCNFPosSYMBOL()

  void CNFMgr::convertFormulaToCNFPosNOT(const ASTNode& varphi, 
                                         ClauseList* defs)
  {
    convertFormulaToCNF(varphi[0], defs);
    info[varphi]->clausespos = COPY(*(info[varphi[0]]->clausesneg));
    reduceMemoryFootprintNeg(varphi[0]);
  } //End of convertFormulaToCNFPosNOT()

  void CNFMgr::convertFormulaToCNFPosAND(const ASTNode& varphi, 
                                         ClauseList* defs) {
    //****************************************
    // (pos) AND ~> UNION
    //****************************************

    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    ClauseList* psi = COPY(*(info[*it]->clausespos));

    for (it++; it != varphi.GetChildren().end(); it++) {
      convertFormulaToCNF(*it, defs);
      CNFInfo* x = info[*it];

      if (sharesPos(*x) == 1) {
        psi->insert(psi->end(),x->clausespos->begin(), x->clausespos->end());
        delete (x->clausespos);
        x->clausespos = NULL;
        if (x->clausesneg == NULL) {
          delete x;
          info.erase(*it);
        }
      } else {
        INPLACE_UNION(psi, *(x->clausespos));
        reduceMemoryFootprintPos(*it);
      }
    }
    info[varphi]->clausespos = psi;
  } //End of convertFormulaToCNFPosAND()

  void CNFMgr::convertFormulaToCNFPosNAND(const ASTNode& varphi, 
                                          ClauseList* defs)
  {
    bool renamesibs = false;
    ClauseList* clauses;
    ClauseList* psi;
    ClauseList* oldpsi;

    //****************************************
    // (pos) NAND ~> PRODUCT NOT
    //****************************************

    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    clauses = info[*it]->clausesneg;
    if (clauses->size() > 1)
      {
        renamesibs = true;
      }
    psi = COPY(*clauses);
    reduceMemoryFootprintNeg(*it);

    for (it++; it != varphi.GetChildren().end(); it++)
      {
        if (renamesibs)
          {
            setDoSibRenamingNeg(*(info[*it]));
          }
        convertFormulaToCNF(*it, defs);
        clauses = info[*it]->clausesneg;
        if (clauses->size() > 1)
          {
            renamesibs = true;
          }
        oldpsi = psi;
        psi = PRODUCT(*psi, *clauses);
        reduceMemoryFootprintNeg(*it);
        DELETE(oldpsi);
      }

    info[varphi]->clausespos = psi;
  } //End of convertFormulaToCNFPosNAND()

  void CNFMgr::convertFormulaToCNFPosOR(const ASTNode& varphi, 
                                        ClauseList* defs)
  {
    bool renamesibs = false;
    ClauseList* clauses;
    ClauseList* psi;
    ClauseList* oldpsi;

    //****************************************
    // (pos) OR ~> PRODUCT
    //****************************************
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    clauses = info[*it]->clausespos;
    if (clauses->size() > 1)
      {
        renamesibs = true;
      }
    psi = COPY(*clauses);
    reduceMemoryFootprintPos(*it);

    for (it++; it != varphi.GetChildren().end(); it++)
      {
        if (renamesibs)
          {
            setDoSibRenamingPos(*(info[*it]));
          }
        convertFormulaToCNF(*it, defs);
        clauses = info[*it]->clausespos;
        if (clauses->size() > 1)
          {
            renamesibs = true;
          }
        oldpsi = psi;
        psi = PRODUCT(*psi, *clauses);
        reduceMemoryFootprintPos(*it);
        DELETE(oldpsi);
      }

    info[varphi]->clausespos = psi;
  } //End of convertFormulaToCNFPosOR()

  void CNFMgr::convertFormulaToCNFPosNOR(const ASTNode& varphi, 
                                         ClauseList* defs)
  {
    //****************************************
    // (pos) NOR ~> UNION NOT
    //****************************************
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    ClauseList* psi = COPY(*(info[*it]->clausesneg));
    reduceMemoryFootprintNeg(*it);
    for (it++; it != varphi.GetChildren().end(); it++)
      {
        convertFormulaToCNF(*it, defs);
        INPLACE_UNION(psi, *(info[*it]->clausesneg));
        reduceMemoryFootprintNeg(*it);
      }

    info[varphi]->clausespos = psi;
  } //End of convertFormulaToCNFPosNOR()

  void CNFMgr::convertFormulaToCNFPosIMPLIES(const ASTNode& varphi, 
                                             ClauseList* defs)
  {
    //****************************************
    // (pos) IMPLIES ~> PRODUCT NOT [0] ; [1]
    //****************************************
    CNFInfo* x0 = info[varphi[0]];
    CNFInfo* x1 = info[varphi[1]];
    convertFormulaToCNF(varphi[0], defs);
    if (x0->clausesneg->size() > 1)
      {
        setDoSibRenamingPos(*x1);
      }
    convertFormulaToCNF(varphi[1], defs);
    ClauseList* psi = PRODUCT(*(x0->clausesneg), *(x1->clausespos));
    reduceMemoryFootprintNeg(varphi[0]);
    reduceMemoryFootprintPos(varphi[1]);
    info[varphi]->clausespos = psi;
  } //End of convertFormulaToCNFPosIMPLIES()

  void CNFMgr::convertFormulaToCNFPosITE(const ASTNode& varphi, 
                                         ClauseList* defs)
  {
    //****************************************
    // (pos) ITE ~> UNION (PRODUCT NOT [0] ; [1])
    //  ; (PRODUCT [0] ; [2])
    //****************************************
    CNFInfo* x0 = info[varphi[0]];
    CNFInfo* x1 = info[varphi[1]];
    CNFInfo* x2 = info[varphi[2]];
    convertFormulaToCNF(varphi[0], defs);
    if (x0->clausesneg->size() > 1)
      {
        setDoSibRenamingPos(*x1);
      }
    convertFormulaToCNF(varphi[1], defs);
    if (x0->clausespos->size() > 1)
      {
        setDoSibRenamingPos(*x2);
      }
    convertFormulaToCNF(varphi[2], defs);
    ClauseList* psi1 = PRODUCT(*(x0->clausesneg), *(x1->clausespos));
    ClauseList* psi2 = PRODUCT(*(x0->clausespos), *(x2->clausespos));
    NOCOPY_INPLACE_UNION(psi1, psi2);
    reduceMemoryFootprintNeg(varphi[0]);
    reduceMemoryFootprintPos(varphi[1]);
    reduceMemoryFootprintPos(varphi[0]);
    reduceMemoryFootprintPos(varphi[2]);

    info[varphi]->clausespos = psi1;
  } //End of convertFormulaToCNFPosITE()

  void CNFMgr::convertFormulaToCNFPosXOR(const ASTNode& varphi, 
                                         ClauseList* defs)
  {
#ifdef CRYPTOMINISAT
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    ClausePtr xor_clause = new vector<const ASTNode*>();

    for (; it != varphi.GetChildren().end(); it++)
      {
        convertFormulaToCNF(*it, defs); // make pos and neg clause set

	//Creating a new variable name for each of the children of the
	//XOR node
	doRenamingPos(*it, defs);
	xor_clause->insert(xor_clause->end(), 
			   ((*(info[*it]->clausespos))[0])->begin(),
			   ((*(info[*it]->clausespos))[0])->end());
      }
    doRenamingPosXor(varphi);
    //doRenamingPos(varphi, defs);
    ASTNode varXorNode = GetNodeFrom_SINGLETON(info[varphi]->clausespos);
    ASTNode NotVarXorNode = bm->CreateNode(NOT, varXorNode);
    xor_clause->push_back(ASTNodeToASTNodePtr(NotVarXorNode));
    clausesxor->push_back(xor_clause);

    ASTVec::const_iterator it2 = varphi.GetChildren().begin();
    for (; it2 != varphi.GetChildren().end(); it2++){
      reduceMemoryFootprintPos(*it2);
      reduceMemoryFootprintNeg(*it2);
    }
#else
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    for (; it != varphi.GetChildren().end(); it++)
      {
        convertFormulaToCNF(*it, defs); // make pos and neg clause sets
      }
    ClauseList* psi = convertFormulaToCNFPosXORAux(varphi, 0, defs);
    info[varphi]->clausespos = psi;
    ASTVec::const_iterator it2 = varphi.GetChildren().begin();
    for (; it2 != varphi.GetChildren().end(); it2++){
      reduceMemoryFootprintPos(*it2);
      reduceMemoryFootprintNeg(*it2);
    }
#endif
  } //End of convertFormulaToCNFPosXOR()

  ClauseList* CNFMgr::convertFormulaToCNFPosXORAux(const ASTNode& varphi, 
                                                   unsigned int idx, 
                                                   ClauseList* defs)
  {
    bool renamesibs;
    ClauseList* psi;
    ClauseList* psi1;
    ClauseList* psi2;

    if (idx == varphi.GetChildren().size() - 2)
      {
        //****************************************
        // (pos) XOR ~> UNION (AND)
        //    (PRODUCT  (OR) [idx]   ;     [idx+1])
        //  ; (PRODUCT NOT   [idx]   ; NOT [idx+1])
        //****************************************
        renamesibs = 
          (info[varphi[idx]]->clausespos)->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingPos(*info[varphi[idx + 1]]);
          }
        renamesibs = 
          (info[varphi[idx]]->clausesneg)->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingNeg(*info[varphi[idx + 1]]);
          }

        psi1 = 
          PRODUCT(*(info[varphi[idx]]->clausespos), 
                  *(info[varphi[idx + 1]]->clausespos));
        psi2 = 
          PRODUCT(*(info[varphi[idx]]->clausesneg), 
                  *(info[varphi[idx + 1]]->clausesneg));
        NOCOPY_INPLACE_UNION(psi1, psi2);

        psi = psi1;
      }
    else
      {
        //****************************************
        // (pos) XOR ~> UNION
        //    (PRODUCT       [idx] ; XOR      [idx+1..])
        //  ; (PRODUCT NOT   [idx] ; NOT XOR  [idx+1..])
        //****************************************
        ClauseList* theta1;
        theta1 = convertFormulaToCNFPosXORAux(varphi, idx + 1, defs);
        renamesibs = theta1->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingPos(*info[varphi[idx]]);
          }
        ClauseList* theta2;
        theta2 = convertFormulaToCNFNegXORAux(varphi, idx + 1, defs);
        renamesibs = theta2->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingNeg(*info[varphi[idx]]);
          }

        psi1 = PRODUCT(*(info[varphi[idx]]->clausespos), *theta1);
        psi2 = PRODUCT(*(info[varphi[idx]]->clausesneg), *theta2);
        DELETE(theta1);
        DELETE(theta2);
        NOCOPY_INPLACE_UNION(psi1, psi2);

        psi = psi1;
      }

    return psi;
  } //End of convertFormulaToCNFPosXORAux()

  void CNFMgr::convertFormulaToCNFNegPred(const ASTNode& varphi, 
                                          ClauseList* defs)
  {

    ASTVec psis;

    ASTVec::const_iterator it = varphi.GetChildren().begin();
    for (; it != varphi.GetChildren().end(); it++)
      {
        convertFormulaToCNF(*it, defs);
        psis.push_back(*(info[*it]->termforcnf));
      }

    info[varphi]->clausesneg = 
      SINGLETON(bm->CreateNode(NOT,
                               bm->CreateNode(varphi.GetKind(), psis)));
  } //End of convertFormulaToCNFNegPred()

  void CNFMgr::convertFormulaToCNFNegFALSE(const ASTNode& varphi, 
                                           ClauseList* defs)
  {
    ASTNode dummy_true_var = bm->CreateSymbol("*TrueDummy*");
    info[varphi]->clausesneg = SINGLETON(dummy_true_var);
  } //End of convertFormulaToCNFNegFALSE()

  void CNFMgr::convertFormulaToCNFNegTRUE(const ASTNode& varphi, 
                                          ClauseList* defs)
  {
    ASTNode dummy_false_var = 
      bm->CreateNode(NOT, bm->CreateSymbol("*TrueDummy*"));
    info[varphi]->clausesneg = SINGLETON(dummy_false_var);
  } //End of convertFormulaToCNFNegTRUE()

  void CNFMgr::convertFormulaToCNFNegBVGETBIT(const ASTNode& varphi, 
                                              ClauseList* defs)
  {
    ClauseList* psi = SINGLETON(bm->CreateNode(NOT, varphi));
    info[varphi]->clausesneg = psi;
  } //End of convertFormulaToCNFNegBVGETBIT()

  void CNFMgr::convertFormulaToCNFNegSYMBOL(const ASTNode& varphi,
                                            ClauseList* defs)
  {
    info[varphi]->clausesneg = SINGLETON(bm->CreateNode(NOT, varphi));
  } //End of convertFormulaToCNFNegSYMBOL()

  void CNFMgr::convertFormulaToCNFNegNOT(const ASTNode& varphi,
                                         ClauseList* defs)
  {
    convertFormulaToCNF(varphi[0], defs);
    info[varphi]->clausesneg = COPY(*(info[varphi[0]]->clausespos));
    reduceMemoryFootprintPos(varphi[0]);
  } //End of convertFormulaToCNFNegNOT()

  void CNFMgr::convertFormulaToCNFNegAND(const ASTNode& varphi,
                                         ClauseList* defs)
  {
    bool renamesibs = false;
    ClauseList* clauses;
    ClauseList* psi;
    ClauseList* oldpsi;

    //****************************************
    // (neg) AND ~> PRODUCT NOT
    //****************************************

    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    clauses = info[*it]->clausesneg;
    if (clauses->size() > 1)
      {
        renamesibs = true;
      }
    psi = COPY(*clauses);
    reduceMemoryFootprintNeg(*it);

    for (it++; it != varphi.GetChildren().end(); it++)
      {
        if (renamesibs)
          {
            setDoSibRenamingNeg(*(info[*it]));
          }
        convertFormulaToCNF(*it, defs);
        clauses = info[*it]->clausesneg;
        if (clauses->size() > 1)
          {
            renamesibs = true;
          }
        oldpsi = psi;
        psi = PRODUCT(*psi, *clauses);
        reduceMemoryFootprintNeg(*it);
        DELETE(oldpsi);
      }

    info[varphi]->clausesneg = psi;
  } //End of convertFormulaToCNFNegAND()

  void CNFMgr::convertFormulaToCNFNegNAND(const ASTNode& varphi,
                                          ClauseList* defs)
  {
    //****************************************
    // (neg) NAND ~> UNION
    //****************************************
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    ClauseList* psi = COPY(*(info[*it]->clausespos));
    reduceMemoryFootprintPos(*it);
    for (it++; it != varphi.GetChildren().end(); it++)
      {
        convertFormulaToCNF(*it, defs);
        INPLACE_UNION(psi, *(info[*it]->clausespos));
        reduceMemoryFootprintPos(*it);
      }

    info[varphi]->clausespos = psi;
  } //End of convertFormulaToCNFNegNAND()

  void CNFMgr::convertFormulaToCNFNegOR(const ASTNode& varphi,
                                        ClauseList* defs)
  {
    //****************************************
    // (neg) OR ~> UNION NOT
    //****************************************
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    ClauseList* psi = COPY(*(info[*it]->clausesneg));
    reduceMemoryFootprintNeg(*it);
    for (it++; it != varphi.GetChildren().end(); it++) {
      convertFormulaToCNF(*it, defs);
      CNFInfo* x = info[*it];

      if (sharesNeg(*x) != 1) {
        INPLACE_UNION(psi, *(x->clausesneg));
        reduceMemoryFootprintNeg(*it);
      } else {
        // If this is the only use of "clausesneg", no reason to make a copy.
        psi->insert(psi->end(), x->clausesneg->begin(),
                    x->clausesneg->end());
        // Copied from reduceMemoryFootprintNeg
        delete x->clausesneg;
        x->clausesneg = NULL;
        if (x->clausespos == NULL) {
          delete x;
          info.erase(*it);
        }
      }

    }

    info[varphi]->clausesneg = psi;
  } //End of convertFormulaToCNFNegOR()

  void CNFMgr::convertFormulaToCNFNegNOR(const ASTNode& varphi,
                                         ClauseList* defs)
  {
    bool renamesibs = false;
    ClauseList* clauses;
    ClauseList* psi;
    ClauseList* oldpsi;

    //****************************************
    // (neg) NOR ~> PRODUCT
    //****************************************
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    convertFormulaToCNF(*it, defs);
    clauses = info[*it]->clausespos;
    if (clauses->size() > 1)
      {
        renamesibs = true;
      }
    psi = COPY(*clauses);
    reduceMemoryFootprintPos(*it);

    for (it++; it != varphi.GetChildren().end(); it++)
      {
        if (renamesibs)
          {
            setDoSibRenamingPos(*(info[*it]));
          }
        convertFormulaToCNF(*it, defs);
        clauses = info[*it]->clausespos;
        if (clauses->size() > 1)
          {
            renamesibs = true;
          }
        oldpsi = psi;
        psi = PRODUCT(*psi, *clauses);
        reduceMemoryFootprintPos(*it);
        DELETE(oldpsi);
      }

    info[varphi]->clausesneg = psi;
  } //End of convertFormulaToCNFNegNOR()

  void CNFMgr::convertFormulaToCNFNegIMPLIES(const ASTNode& varphi,
                                             ClauseList* defs)
  {
    //****************************************
    // (neg) IMPLIES ~> UNION [0] ; NOT [1]
    //****************************************
    CNFInfo* x0 = info[varphi[0]];
    CNFInfo* x1 = info[varphi[1]];
    convertFormulaToCNF(varphi[0], defs);
    convertFormulaToCNF(varphi[1], defs);
    ClauseList* psi = UNION(*(x0->clausespos), *(x1->clausesneg));
    info[varphi]->clausesneg = psi;
    reduceMemoryFootprintPos(varphi[0]);
    reduceMemoryFootprintNeg(varphi[1]);
  } //End of convertFormulaToCNFNegIMPLIES()

  void CNFMgr::convertFormulaToCNFNegITE(const ASTNode& varphi,
                                         ClauseList* defs)
  {
    //****************************************
    // (neg) ITE ~> UNION (PRODUCT NOT [0] ; NOT [1])
    //  ; (PRODUCT [0] ; NOT [2])
    //****************************************
    CNFInfo* x0 = info[varphi[0]];
    CNFInfo* x1 = info[varphi[1]];
    CNFInfo* x2 = info[varphi[2]];
    convertFormulaToCNF(varphi[0], defs);
    if (x0->clausesneg->size() > 1)
      {
        setDoSibRenamingNeg(*x1);
      }
    convertFormulaToCNF(varphi[1], defs);
    if (x0->clausespos->size() > 1)
      {
        setDoSibRenamingNeg(*x2);
      }
    convertFormulaToCNF(varphi[2], defs);
    ClauseList* psi1 = PRODUCT(*(x0->clausesneg), *(x1->clausesneg));
    ClauseList* psi2 = PRODUCT(*(x0->clausespos), *(x2->clausesneg));
    NOCOPY_INPLACE_UNION(psi1, psi2);
    reduceMemoryFootprintNeg(varphi[0]);
    reduceMemoryFootprintNeg(varphi[1]);
    reduceMemoryFootprintPos(varphi[0]);
    reduceMemoryFootprintNeg(varphi[2]);

    info[varphi]->clausesneg = psi1;
  } //End of convertFormulaToCNFNegITE()

  void CNFMgr::convertFormulaToCNFNegXOR(const ASTNode& varphi,
                                         ClauseList* defs)
  {
    ASTVec::const_iterator it = varphi.GetChildren().begin();
    for (; it != varphi.GetChildren().end(); it++)
      {
        convertFormulaToCNF(*it, defs); // make pos and neg clause sets
      }
    ClauseList* psi = convertFormulaToCNFNegXORAux(varphi, 0, defs);
    info[varphi]->clausesneg = psi;
    ASTVec::const_iterator it2 = varphi.GetChildren().begin();
    for (; it2 != varphi.GetChildren().end(); it2++){
      reduceMemoryFootprintPos(*it2);
      reduceMemoryFootprintNeg(*it2);
    }
  } //End of convertFormulaToCNFNegXOR()

  ClauseList* CNFMgr::convertFormulaToCNFNegXORAux(const ASTNode& varphi,
                                                   unsigned int idx, 
                                                   ClauseList* defs)
  {
    bool renamesibs;
    ClauseList* psi;
    ClauseList* psi1;
    ClauseList* psi2;

    if (idx == varphi.GetChildren().size() - 2)
      {

        //****************************************
        // (neg) XOR ~> UNION
        //    (PRODUCT NOT   [idx]   ;     [idx+1])
        //  ; (PRODUCT       [idx]   ; NOT [idx+1])
        //****************************************
        convertFormulaToCNF(varphi[idx], defs);
        renamesibs = 
          (info[varphi[idx]]->clausesneg)->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingPos(*info[varphi[idx + 1]]);
          }

        convertFormulaToCNF(varphi[idx], defs);
        renamesibs =
          (info[varphi[idx]]->clausespos)->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingNeg(*info[varphi[idx + 1]]);
          }

        psi1 = 
          PRODUCT(*(info[varphi[idx]]->clausesneg),
                  *(info[varphi[idx + 1]]->clausespos));
        psi2 = 
          PRODUCT(*(info[varphi[idx]]->clausespos),
                  *(info[varphi[idx + 1]]->clausesneg));
        NOCOPY_INPLACE_UNION(psi1, psi2);

        psi = psi1;
      }
    else
      {
        //****************************************
        // (neg) XOR ~> UNION
        //    (PRODUCT NOT   [idx] ; XOR      [idx+1..])
        //  ; (PRODUCT       [idx] ; NOT XOR  [idx+1..])
        //****************************************
        ClauseList* theta1;
        theta1 = convertFormulaToCNFPosXORAux(varphi, idx + 1, defs);
        renamesibs = theta1->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingNeg(*info[varphi[idx]]);
          }
        convertFormulaToCNF(varphi[idx], defs);

        ClauseList* theta2;
        theta2 = convertFormulaToCNFNegXORAux(varphi, idx + 1, defs);
        renamesibs = theta2->size() > 1 ? true : false;
        if (renamesibs)
          {
            setDoSibRenamingPos(*info[varphi[idx]]);
          }

        psi1 = PRODUCT(*(info[varphi[idx]]->clausesneg), *theta1);
        psi2 = PRODUCT(*(info[varphi[idx]]->clausespos), *theta2);
        DELETE(theta1);
        DELETE(theta2);
        NOCOPY_INPLACE_UNION(psi1, psi2);

        psi = psi1;
      }

    return psi;
  } //End of convertFormulaToCNFNegXORAux()

  //########################################
  //########################################
  // utilities for reclaiming memory.

  void CNFMgr::reduceMemoryFootprintPos(const ASTNode& varphi)
  {
    CNFInfo* x = info[varphi];
    if (sharesPos(*x) == 1)
      {
        DELETE(x->clausespos);
        x->clausespos = NULL;
        if (x->clausesneg == NULL)
          {
            delete x;
            info.erase(varphi);
          }
      }
  } //End of reduceMemoryFootprintPos()

  void CNFMgr::reduceMemoryFootprintNeg(const ASTNode& varphi)
  {
    CNFInfo* x = info[varphi];
    if (sharesNeg(*x) == 1)
      {
        DELETE(x->clausesneg);
        x->clausesneg = NULL;
        if (x->clausespos == NULL)
          {
            delete x;
            info.erase(varphi);
          }
      }
  } //End of reduceMemoryFootprintNeg()

  //########################################
  //########################################

  ASTNode* CNFMgr::ASTNodeToASTNodePtr(const ASTNode& varphi)
  {
    ASTNode* psi;

    if (store.find(varphi) != store.end())
      {
        psi = store[varphi];
      }
    else
      {
        psi = new ASTNode(varphi);
        store[varphi] = psi;
      }

    return psi;
  } //End of ASTNodeToASTNodePtr()

  //########################################
  //########################################

  void CNFMgr::cleanup(const ASTNode& varphi)
  {
    delete info[varphi]->clausespos;
    CNFInfo* toDelete = info[varphi]; // get the thing to delete.
    info.erase(varphi);               // remove it from the hashtable
    delete toDelete;


    ASTNodeToCNFInfoMap::const_iterator it1 = info.begin();
    for (; it1 != info.end(); it1++)
      {
        CNFInfo* x = it1->second;
        if (x->clausespos != NULL)
          {
            DELETE(x->clausespos);
          }
        if (x->clausesneg != NULL)
          {
            if (!isTerm(*x))
              {
                DELETE(x->clausesneg);
              }
          }
        delete x;
      }

    info.clear();
  } //End of cleanup()

  //########################################
  //########################################
  // constructor

  CNFMgr::CNFMgr(STPMgr *bmgr)
  {
    bm = bmgr;
    clausesxor = new ClauseList();
  }

  //########################################
  //########################################
  // destructor
  CNFMgr::~CNFMgr()
  {
    ASTNodeToASTNodePtrMap::const_iterator it1 = store.begin();
    for (; it1 != store.end(); it1++)
      {
        delete it1->second;
      }
    store.clear();
    
    //FIXME: Delete clausesxor
  }

  //########################################
  //########################################
  // top-level conversion function

  ClauseList* CNFMgr::convertToCNF(const ASTNode& varphi)
  {
    bm->GetRunTimes()->start(RunTimes::CNFConversion);
    scanFormula(varphi, true);
    ASTNode dummy_true_var = bm->CreateSymbol("*TrueDummy*");
    ClauseList* defs = SINGLETON(dummy_true_var);
    convertFormulaToCNF(varphi, defs);
    ClauseList* top = info[varphi]->clausespos;
    defs->insert(defs->begin() + 1, top->begin(), top->end());

    cleanup(varphi);
    bm->GetRunTimes()->stop(RunTimes::CNFConversion);
    if (bm->UserFlags.stats_flag)
      {
        cerr << "Number of clauses:" << defs->size() << endl;
        //PrintClauseList(cout, *defs);
      }

    return defs;
  }//End of convertToCNF()

  //All XOR clauses are stored here. Return it after CNF translation
  ClauseList* CNFMgr::ReturnXorClauses(void)
  {
    return clausesxor;
  }

  void CNFMgr::DELETE(ClauseList* varphi)
  {
    ClauseList::const_iterator it = varphi->begin();
    for (; it != varphi->end(); it++)
      {
        delete *it;
      }

    delete varphi;
  } //End of DELETE()


  void CNFMgr::PrintClauseList(ostream& os, ClauseList& cll)
  {
    int num_clauses = cll.size();
    os << "Clauses: " 
       << endl 
       << "=========================================" << endl;
    for (int i = 0; i < num_clauses; i++)
      {
        os << "Clause " 
           << i << endl 
           << "-------------------------------------------" << endl;
        LispPrintVecSpecial(os, *cll[i], 0);
        os << endl 
           << "-------------------------------------------" << endl;
      }
  } //end of PrintClauseList()  
} // end namespace BEEV
