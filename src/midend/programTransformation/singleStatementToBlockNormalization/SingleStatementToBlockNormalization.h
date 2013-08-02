class SingleStatementToBlockNormalizer: public AstSimpleProcessing {
    private:
        ROSE_VisitorPatternDefaultBase * singleStatementToBlock;
    public:
        SingleStatementToBlockNormalizer();
        void Normalize(SgNode * node);
        void NormalizeWithinFile(SgNode * node);
        void NormalizeInputFiles(SgProject * project);
        virtual ~SingleStatementToBlockNormalizer();
    protected:
        void visit(SgNode * node);
};

