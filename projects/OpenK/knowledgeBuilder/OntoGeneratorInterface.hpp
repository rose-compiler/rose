/*
 * This header defines the abstract class that provides the interface methods
 * for handling the knowledge generation, using different formats.
 *
 * It also defines subclasses that support specific format output.
 * OntoGeneratorPl is an example of generating knowledge store as prolog predicates.
 * OntoTurtle is an example to generate knowledge in RDF turtle (Terse RDF Triple Language) format.
 * OntoOwlFunctional generates OWL functional syntax
 *
 * Usage:
 *   OntoGeneratorInterface *onto;
 *   onto = new SomeSubClass();
 *   onto->..._register(...) //optional
 *   onto->open_store(...); // must after register...
 *   // create onto with ClassAssertion, etc ...
 *   onto->close_store()
 */

#ifndef ONTO_GENERATOR_INTERFACE_HPP
#define ONTO_GENERATOR_INTERFACE_HPP

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>

//#define SWIPL 1

#ifdef SWIPL
#include <SWI-Prolog.h>
#include <SWI-cpp.h>
#endif

#define QUOTE(x) (std::string("\"" + (x) + "\""))
#define ANGLEQUOTE(s) (std::string("<") + (s) + ">")
#define ATOM(x) (std::string("'") + (x) + "'")

class OntoGeneratorInterface {
protected:
	std::map<std::string, std::string> prefixes;
	std::string ontologyName;
	std::vector<std::string> importOWLs;

public:
	OntoGeneratorInterface() {
	}
	;
	virtual ~OntoGeneratorInterface() {
	}
	;

        // assert a relation
	virtual void rdf_assert(std::string sub, std::string pred,
			std::string obj) {
	}
	;
	virtual void store(std::string filename) {
	}
	; 
        // open a file to store the ontology
	virtual void open_store(std::string filename) = 0;

        // close the buffer, store to a file
	virtual void close_store() = 0;

        // register a short_prefix, full_IRI pair
	void prefix_register(std::string prefix, std::string fullIRI) {
		prefixes[prefix] = fullIRI;
	}

        // Set ontology name
	void ontoName_register(std::string ontoName) {
		ontologyName = ontoName;
	}

        // Store imported OWL files
	void import_register(std::string importOWL) {
		importOWLs.push_back(importOWL);
	}

        // assert an individual for a concept
	virtual void ClassAssertion(std::string concept, std::string ind) = 0;

        // assert an object property (predicate) between subject and object: (sub pred obj)
	virtual void ObjectPropertyAssertion(std::string pred, std::string sub,
			std::string obj) = 0;

	virtual void DataPropertyAssertion(std::string pred, std::string sub,
			std::string obj) = 0;

        // expand string using prefix to be using full RUL
	std::string expand(std::string alias) {
		// find the first :, replace substr before : (include) with full url, 'quote it'
		size_t pos = 0;
		std::string url = alias;
		if ((pos = alias.find_first_of(":")) != std::string::npos) {
			url = prefixes[alias.substr(0, pos+1)] + alias.substr(pos+1);
		}
		return url;
	}

};

/*
 * The turtle format support (.ttl)
 *
 * */

class OntoTurtle: public OntoGeneratorInterface {
public:
	void open_store(std::string filename) {
		rdf_ttl.open(filename.c_str(), std::ios::out);

		if (!prefixes.empty()) {
			for (std::map<std::string, std::string>::iterator pi =
					prefixes.begin(), pe = prefixes.end(); pi != pe; ++pi) {
				rdf_ttl << "@prefix " << pi->first << " "
						<< ANGLEQUOTE(pi->second) << " .\n";
			}
		}
		rdf_ttl << "\n";
	}

	void close_store() {
	}

	void rdf_assert(std::string sub, std::string pred, std::string obj) {
		rdf_ttl << sub << " " << pred << " " << obj << " .\n";
	}

	void ClassAssertion(std::string className, std::string individualName) {
		rdf_assert(individualName, "rdf:type", className);
	}

	void ObjectPropertyAssertion(std::string relation, std::string subject,
			std::string object) {
		rdf_assert(subject, relation, object);
	}

	void DataPropertyAssertion(std::string pred, std::string subject,
			std::string object) {
		rdf_assert(subject, pred, QUOTE(object));
	}

private:
	std::ofstream rdf_ttl;
};

#ifdef SWIPL
/*
 * Using the embedded SWI-Prolog with library(semweb/rdf_db)
 * */
#ifdef READLINE
static void
install_readline(int argc, char**argv)
{	PL_install_readline();
}
#endif

class OntoEmPl: public OntoGeneratorInterface {
	PlEngine *ple;
	bool alive;
public:
	OntoEmPl(bool flag) :
			ple(NULL),
			alive(flag) {
#ifdef READLINE
		PL_initialise_hook(install_readline);
#endif
		char *av[10];
		int ac = 0;
		av[ac++] = "-q";
		av[ac] = NULL;
		if (ple == NULL) {
			ple = new PlEngine(ac, av);
		} else {
			std::cout << "Prolog engine start failed\n";
			exit(1);
		}
	}

	~OntoEmPl() {
		if (ple)
			delete ple;
	}

	void open_store(std::string filename) {
		PlFrame fr;
		PlCall("set_prolog_flag(verbose, silent).");
		PlCall("use_module(library(semweb/rdf_db)).");
		PlCall("rdf_register_prefix(c,'http://www.semanticweb.org/yzhao30/ontologies/2015/7/c#').");
	}

	void close_store() {
		if (alive) {
			PL_halt(PL_toplevel() ? 0 : 1);
		}
	}

	void rdf_assert(std::string sub, std::string pred, std::string obj) {
		PlFrame fr;
		PlTermv av(3);
		av[0] = expand(sub).c_str();
		av[1] = expand(pred).c_str();
		av[2] = expand(obj).c_str();
		PlCall("rdf_assert", av);
	}

	void ClassAssertion(std::string className, std::string individualName) {
		rdf_assert(individualName, "rdf:type", className);
	}

	void ObjectPropertyAssertion(std::string relation, std::string subject,
			std::string object) {
		rdf_assert(subject, relation, object);
	}

	void DataPropertyAssertion(std::string pred, std::string subject,
			std::string object) {
		rdf_assert(subject, pred, QUOTE(object));
	}

};
#endif // ifedf SWIPL

/*
 * Store the knowledge base as prolog predicates (.pl)
 * */

class PredicateCache {
public:
	PredicateCache() {
	}
	;
	~PredicateCache() {
	}
	;

	void assertPredicate(std::string functor_str, std::string predicate_str) {
		if (predicates.find(functor_str) == predicates.end()) {
			predicates[functor_str];
		}
		predicates[functor_str].push_back(predicate_str);
	}

	void ClassAssertion(std::string className, std::string individualName) {
		assertPredicate("classAssertion/2",
				"classAssertion(" + className + ", " + individualName + ").");
	}

	void NegativeObjectPropertyAssertion(std::string relation,
			std::string subject, std::string object) {
		assertPredicate("negativeObjectPropertyAssertion/3",
				"negativeObjectPropertyAssertion(" + relation + ", " + subject
						+ ", " + object + ").");
	}
	void ObjectPropertyAssertion(std::string relation, std::string subject,
			std::string object) {
		assertPredicate("objectPropertyAssertion/3",
				"objectPropertyAssertion(" + relation + ", " + subject + ", "
						+ object + ").");
	}

	void dump(std::string filename) {
		std::ofstream kb_file(filename.c_str(), std::ios::out);

		std::map<std::string, std::vector<std::string> >::iterator pi, pe;
		std::vector<std::string>::iterator it, ie;
		for (pi = predicates.begin(), pe = predicates.end(); pi != pe; ++pi) {
			kb_file << std::endl;
			kb_file << ":- dynamic " << pi->first << "." << std::endl;
			kb_file << std::endl;
			for (it = (pi->second).begin(), ie = (pi->second).end(); it != ie;
					++it) {
				kb_file << *it << std::endl;
			}
		}
	}

private:
	std::map<std::string, std::vector<std::string> > predicates;

};

class OntoPl: public OntoGeneratorInterface {
	PredicateCache predCache;
	std::string outfile;
public:

	void store(std::string filename) {
		predCache.dump(filename);
	}
	void open_store(std::string filename) {
		outfile = filename;
	}
	void close_store() {
		store(outfile);
	}

	void ClassAssertion(std::string className, std::string individualName) {
		predCache.ClassAssertion(ATOM(className), ATOM(individualName));
	}

	void ObjectPropertyAssertion(std::string relation, std::string subject,
			std::string object) {
		predCache.ObjectPropertyAssertion(ATOM(relation), ATOM(subject),
				ATOM(object));
	}
	void DataPropertyAssertion(std::string pred, std::string sub,
			std::string obj) {
		ObjectPropertyAssertion(pred, sub, obj);
	}

};

/*
 * The OWL functional syntax support class
 * */

// select what classes, properties should be exported
// default: export all
class OntologySelector {
	bool enable_all;
	std::set<std::string> enable_set;

public:
	OntologySelector() :
			enable_all(true) {
	}
	OntologySelector(std::set<std::string> &enable_set) :
			enable_all(false) {
		this->enable_set = enable_set;
	}
	bool check(std::string key) {
		return (enable_all || (enable_set.find(key) != enable_set.end()));
	}
};

class OntoOwlFunctional: public OntoGeneratorInterface {
	std::set<std::string> inds; // Only declare once
	OntologySelector * ontology_sel;

	std::ofstream owl_file;

	void open_store(std::string outfile) {
		owl_file.open(outfile.c_str(), std::ios::out);
		// Write Prefix
		if (owl_file.is_open()) {
			for (std::map<std::string, std::string>::iterator it =
					prefixes.begin(), ie = prefixes.end(); it != ie; ++it) {
				owl_file << "Prefix(" << it->first << "=" << ANGLEQUOTE(it->second)
						<< ")\n";

			}
			owl_file << "\n";
			// Write Ontology
			owl_file << "Ontology(" << ANGLEQUOTE(ontologyName) << "\n";
			// Write Import
			if (!importOWLs.empty()) {
				for (std::vector<std::string>::iterator it = importOWLs.begin(),
						ie = importOWLs.end(); it != ie; ++it) {
					owl_file << "Import(" << ANGLEQUOTE(*it) << ")\n";
				}
			}
			owl_file << "\n";
		}
	}

	void close_store() {
		if (owl_file.is_open()) {
			owl_file << "\n)\n";
		}
		owl_file.close();
	}

	void DeclarationNamedIndividual(std::string ind_name) {
		if (inds.find(ind_name) == inds.end()) {
			owl_file << "Declaration( NamedIndividual( " << ind_name
					<< " ) )\n";
			inds.insert(ind_name);
		}
	}

	// only declare individual with ":" prefix, that is, assume individuals
	// with empty prefix are created in current owl
	void DeclarationNamedIndividual(std::string ind_name,
			std::string ind_prefix) {
		if (ind_prefix == "") {
			DeclarationNamedIndividual(":" + ind_name);
		}
	}

public:
	OntoOwlFunctional() {
		ontology_sel = new OntologySelector();
	}

	~OntoOwlFunctional() {
		delete ontology_sel;
	}

	void ClassAssertion(std::string class_name, std::string ind_name) {
		if (ontology_sel->check(class_name)) {
			DeclarationNamedIndividual(ind_name);
			owl_file << "ClassAssertion( " << class_name << " " << ind_name
					<< " )\n";
		}
	}

	void ObjectPropertyAssertion(std::string obj_property, std::string subject,
			std::string object) {
		if (ontology_sel->check(obj_property)) {
			DeclarationNamedIndividual(subject);
			DeclarationNamedIndividual(object);
			owl_file << "ObjectPropertyAssertion( " << obj_property << " "
					<< subject << " " << object << " )\n";
		}
	}

	// void NegativeObjectPropertyAssertion(std::string obj_property, std::string subject, std::string object);

	void DataPropertyAssertion(std::string data_property, std::string subject,
			std::string value) {
		if (ontology_sel->check(data_property)) {
			DeclarationNamedIndividual(subject);
			owl_file << "DataPropertyAssertion( " << data_property << " "
					<< subject << " " << "\"" + value + "\"" << " )\n";
		}
	}
};

#endif // ONTO_GENERATOR_INTERFACE_HPP
